#version 460

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

// #define CUSTOM_SCENE_DDGI_REF_0

#include "Lightmap.glsl"

layout(location = 0) rayPayloadEXT Hit hit;
layout(location = 1) rayPayloadEXT Hit primaryHit;
layout(location = 2) rayPayloadEXT Hit secondaryHit;

void pcgHash(inout uint seed) {
	uint state = seed * 747796405 + 2891336453;
	uint word = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;

	seed = (word >> 22) ^ word;
}

float randomScalar(inout uint seed) {
	pcgHash(seed);

	return seed / FLOAT_MAX;
}

vec3 randomUnitDirection(inout uint seed) {
	return normalize(vec3(randomScalar(seed), randomScalar(seed), randomScalar(seed)) * 2.0 - 1.0);
}

vec3 sampleDirectLight(Ray primaryRay, inout uint seed) {
	const float pdfDiffuse = 1.0 / (4.0 * PI);

#if defined(CUSTOM_SCENE_DDGI_REF_0)
	vec3 p = vec3(0.0, 64.0, 164.0);
	vec3 op = p - primaryRay.origin;
	float pdfLight = 1.0;

	Ray secondaryRay = primaryRay;
	secondaryRay.direction = normalize(op);

	if (dot(primaryHit.vertexNormal, secondaryRay.direction) < 0.0) {
		return vec3(0.0, 0.0, 0.0);
	}

	traceRayEXT(accelerationStructure, gl_RayFlagsOpaqueEXT | gl_RayFlagsCullBackFacingTrianglesEXT, 0xff, 0, 0, 0, secondaryRay.origin, secondaryRay.tMin, secondaryRay.direction, secondaryRay.tMax, 2);

	if (secondaryHit.t < length(op)) {
		return vec3(0.0, 0.0, 0.0);
	}

	float angleA = dot(secondaryRay.direction, primaryHit.vertexNormal);
	float angleB = -1.0;
	float G = 1.0 / dot(op, op) * angleA * max(0.001, abs(angleB));
	float h = pdfLight / (pdfLight + pdfDiffuse * G);

	return vec3(256.0) * G / pdfLight * h;
#else
	if (arguments.emissiveTriangleCount == 0) {
		return vec3(0.0, 0.0, 0.0);
	}

	uint emissiveTriangleIndexIndex = uint(randomScalar(seed) * arguments.emissiveTriangleCount);
	uint emissiveTriangleIndex = emissiveTriangleIndexBuffer[emissiveTriangleIndexIndex];

	Triangle triangle_ = triangleBuffer[emissiveTriangleIndex];

	vec2 a = lightmapCoordinateBuffer[triangle_.lightmapCoordinateIndexA];
	vec2 b = lightmapCoordinateBuffer[triangle_.lightmapCoordinateIndexB];
	vec2 c = lightmapCoordinateBuffer[triangle_.lightmapCoordinateIndexC];

	float u = randomScalar(seed);
	float v = randomScalar(seed);

	if (u + v > 1.0) {
		u = 1.0 - u;
		v = 1.0 - v;
	}

	float w = 1.0 - u - v;
	ivec2 coordinate = ivec2((a * u + b * v + c * w) * arguments.atlasSize);

	vec3 p = imageLoad(gBufferPositionImage, coordinate).rgb;
	vec3 op = p - primaryRay.origin;
	float area = imageLoad(gBufferSmoothPositionImage, coordinate).a;
	float pdfLight = 1.0 / (area * arguments.emissiveTriangleCount);

	Ray secondaryRay = primaryRay;
	secondaryRay.direction = normalize(op);

	if (dot(primaryHit.vertexNormal, secondaryRay.direction) < 0.0) {
		return vec3(0.0, 0.0, 0.0);
	}

	traceRayEXT(accelerationStructure, gl_RayFlagsOpaqueEXT | gl_RayFlagsCullBackFacingTrianglesEXT, 0xff, 0, 0, 0, secondaryRay.origin, secondaryRay.tMin, secondaryRay.direction, secondaryRay.tMax, 2);

	if (secondaryHit.triangleIndex != triangle_.index) {
		return vec3(0.0, 0.0, 0.0);
	}

	Material material = materialBuffer[secondaryHit.materialIndex];

	float angleA = dot(secondaryRay.direction, primaryHit.vertexNormal);
	float angleB = dot(secondaryRay.direction, secondaryHit.vertexNormal);
	float G = 1.0 / dot(op, op) * angleA * max(0.001, abs(angleB));
	float h = pdfLight / (pdfLight + pdfDiffuse * G);

	return material.emissiveColor * material.emissiveIntensity * G / pdfLight * h;
#endif
}

vec3 traceRay(inout uint seed) {
	vec3 light = vec3(0.0, 0.0, 0.0);
	vec3 throughput = vec3(1.0, 1.0, 1.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		Material material = materialBuffer[primaryHit.materialIndex];

		throughput *= material.albedo;

		Ray primaryRay;
		primaryRay.origin = primaryHit.position + sign(primaryHit.vertexNormal) * abs(primaryHit.position * ADAPTIVE_BIAS);
		primaryRay.tMin = 0.0;
		primaryRay.direction = normalize(primaryHit.vertexNormal + randomUnitDirection(seed));

		// TODO(asset-builder/lightmapper): Use the scene bounds.
		primaryRay.tMax = 2048.0;

		vec3 directLight = sampleDirectLight(primaryRay, seed);
		vec3 brdf = material.albedo / PI;

		light += directLight * throughput * brdf;

		traceRayEXT(accelerationStructure, gl_RayFlagsOpaqueEXT | gl_RayFlagsCullBackFacingTrianglesEXT, 0xff, 0, 0, 0, primaryRay.origin, primaryRay.tMin, primaryRay.direction, primaryRay.tMax, 1);

		if (primaryHit.t < 0.0) {
			light += texture(environmentMap, primaryRay.direction).rgb * throughput;

			break;
		}

		bounceIndex += 1;
	}

	return light;
}

bool adjustSamplePosition(inout vec3 position, vec3 triangleNormal, float diagonalLuxelSize) {
	vec3 a = normalize(cross(position, triangleNormal));
	vec3 b = normalize(cross(a, triangleNormal));

	Ray ray;
	ray.origin = position + sign(triangleNormal) * abs(position * ADAPTIVE_BIAS);
	ray.tMin = 0.0;
	ray.direction = a;
	ray.tMax = diagonalLuxelSize * 0.5;

	traceRayEXT(accelerationStructure, gl_RayFlagsOpaqueEXT | gl_RayFlagsCullFrontFacingTrianglesEXT, 0xff, 0, 0, 0, ray.origin, ray.tMin, ray.direction, ray.tMax, 0);

	if (hit.t > 0.0) {
		position = hit.position + sign(hit.triangleNormal) * abs(hit.position * ADAPTIVE_BIAS);

		return true;
	}

	ray.direction = b;

	traceRayEXT(accelerationStructure, gl_RayFlagsOpaqueEXT | gl_RayFlagsCullFrontFacingTrianglesEXT, 0xff, 0, 0, 0, ray.origin, ray.tMin, ray.direction, ray.tMax, 0);

	if (hit.t > 0.0) {
		position = hit.position + sign(hit.triangleNormal) * abs(hit.position * ADAPTIVE_BIAS);

		return true;
	}

	ray.direction = -a;

	traceRayEXT(accelerationStructure, gl_RayFlagsOpaqueEXT | gl_RayFlagsCullFrontFacingTrianglesEXT, 0xff, 0, 0, 0, ray.origin, ray.tMin, ray.direction, ray.tMax, 0);

	if (hit.t > 0.0) {
		position = hit.position + sign(hit.triangleNormal) * abs(hit.position * ADAPTIVE_BIAS);

		return true;
	}

	ray.direction = -b;

	traceRayEXT(accelerationStructure, gl_RayFlagsOpaqueEXT | gl_RayFlagsCullFrontFacingTrianglesEXT, 0xff, 0, 0, 0, ray.origin, ray.tMin, ray.direction, ray.tMax, 0);

	if (hit.t > 0.0) {
		position = hit.position + sign(hit.triangleNormal) * abs(hit.position * ADAPTIVE_BIAS);

		return true;
	}

	return false;
}

void main() {
	ivec2 coordinate = ivec2(gl_LaunchIDEXT.xy);

	vec4 gBufferPosition = imageLoad(gBufferPositionImage, coordinate);
	uint triangleIndex = uint(gBufferPosition.a);

	if (triangleIndex == 0) {
		return;
	}

	vec3 position = gBufferPosition.rgb;
	vec4 gBufferTriangleNormal = imageLoad(gBufferTriangleNormalImage, coordinate);
	vec3 triangleNormal = gBufferTriangleNormal.rgb;
	float diagonalLuxelSize = uint(gBufferTriangleNormal.a);

	adjustSamplePosition(position, triangleNormal, diagonalLuxelSize);

	vec4 gBufferSmoothPosition = imageLoad(gBufferSmoothPositionImage, coordinate);
	vec3 smoothPosition = gBufferSmoothPosition.rgb;
	vec4 gBufferVertexNormal = imageLoad(gBufferVertexNormalImage, coordinate);
	vec3 vertexNormal = gBufferVertexNormal.rgb;
	uint materialIndex = uint(gBufferVertexNormal.a);

	Material material = materialBuffer[materialIndex];

	if (material.emissiveIntensity > 0.0) {
		imageStore(intermediateLightmapAtlas, coordinate, vec4(material.emissiveColor * material.emissiveIntensity, 1.0));

		return;
	}

	uint seed = coordinate.y * arguments.atlasSize.x + coordinate.x;

	Hit hit;
	hit.position = position;
	hit.triangleNormal = triangleNormal;
	hit.triangleIndex = triangleIndex - 1;
	hit.vertexNormal = vertexNormal;
	hit.materialIndex = materialIndex;

	vec3 light = vec3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	while (sampleIndex < SAMPLE_COUNT) {
		primaryHit = hit;

		light += traceRay(seed);

		sampleIndex += 1;
	}

	light /= SAMPLE_COUNT;
	light = pow(light, vec3(1.0 / 2.2));

	imageStore(intermediateLightmapAtlas, coordinate, vec4(light, 1.0));
}