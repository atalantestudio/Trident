#version 460

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Lightmap.glsl"

bool rayTriangleIntersectionFrontFacing(Ray ray, Triangle triangle_, inout float t, inout vec3 barycentricCoordinate) {
	const float E = 1e-8;

	vec3 a = triangle_.vertices[0].position;
	vec3 b = triangle_.vertices[1].position;
	vec3 c = triangle_.vertices[2].position;
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 normal = cross(ab, ac);
	float d = -dot(normal, ray.direction);

	if (d < E) {
		return false;
	}

	vec3 ao = ray.origin - a;
	float inverseD = 1.0 / d;

	t = dot(ao, normal) * inverseD;

	if (t < 0.0) {
		return false;
	}

	vec3 dao = cross(ao, ray.direction);
	float u = dot(ac, dao) * inverseD;

	if (u < 0.0) {
		return false;
	}

	float v = -dot(ab, dao) * inverseD;

	if (v < 0.0) {
		return false;
	}

	float w = 1 - u - v;

	if (w < 0.0) {
		return false;
	}

	barycentricCoordinate = vec3(w, u, v);

	return true;
}

// TODO: Use rayPayloadInEXT?
layout(location = 0) rayPayloadEXT Hit _primaryHit;

/* Hit rchit(Ray ray, uint triangleIndex, Triangle triangle_, float distance, vec3 barycentricCoordinate) {
	ivec2 lightmapCoordinate = ivec2((
		triangle_.vertices[0].lightmapCoordinate * barycentricCoordinate.x +
		triangle_.vertices[1].lightmapCoordinate * barycentricCoordinate.y +
		triangle_.vertices[2].lightmapCoordinate * barycentricCoordinate.z
	) * arguments.atlasSize);

	vec4 triangleNormalData = imageLoad(gBufferTriangleNormalImage, lightmapCoordinate);
	vec4 vertexNormalData = imageLoad(gBufferVertexNormalImage, lightmapCoordinate);

	Hit hit;
	hit.position = ray.origin + ray.direction * distance;
	hit.distance = distance;
	hit.triangleNormal = triangleNormalData.rgb;
	hit.materialIndex = uint(vertexNormalData.a);
	hit.vertexNormal = vertexNormalData.rgb;

	return hit;
} */

/* Hit rmiss() {
	Hit hit;
	hit.distance = -1.0;

	return hit;
} */

Hit rintFrontFacing(Ray ray) {
	traceRayEXT(tlas, gl_RayFlagsNoneEXT, 0xff, 0, 0, 0, ray.origin, 0.001, ray.direction, 2048.0, 0);

	return _primaryHit;

	/* float closestDistance = FLOAT_MAX;
	uint intersectingTriangleIndex = arguments.triangleCount;
	Triangle intersectingTriangle;
	vec3 intersectingBarycentricCoordinate;
	uint triangleIndex = 0;

	while (triangleIndex < arguments.triangleCount) {
		Triangle triangle_ = loadTriangle(triangleIndex);
		float distance;
		vec3 barycentricCoordinate;

		if (rayTriangleIntersectionFrontFacing(ray, triangle_, distance, barycentricCoordinate)) {
			if (distance < closestDistance) {
				closestDistance = distance;
				intersectingTriangleIndex = triangleIndex;
				intersectingTriangle = triangle_;
				intersectingBarycentricCoordinate = barycentricCoordinate;
			}
		}

		triangleIndex += 1;
	}

	if (intersectingTriangleIndex < arguments.triangleCount) {
		return rchit(ray, intersectingTriangleIndex, intersectingTriangle, closestDistance, intersectingBarycentricCoordinate);
	}

	return rmiss(); */
}

vec3 rgen(Hit primaryHit, inout uint seed) {
	vec3 incomingLight = vec3(0.0, 0.0, 0.0);
	vec3 throughput = vec3(1.0, 1.0, 1.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		Material material = materialBuffer[primaryHit.materialIndex];

		vec3 o = primaryHit.position + sign(primaryHit.vertexNormal) * abs(primaryHit.position * ADAPTIVE_BIAS);

		Ray primaryRay;
		primaryRay.origin = o;
		primaryRay.direction = normalize(primaryHit.vertexNormal + randomUnitDirection(seed));

		primaryHit = rintFrontFacing(primaryRay);

		if (primaryHit.distance < 0.0) {
			incomingLight += arguments.skyColor * throughput;

			break;
		}

		throughput *= material.albedo;

		bounceIndex += 1;
	}

	return incomingLight;
}

void main() {
	ivec2 coordinate = ivec2(gl_LaunchIDEXT.xy);

	vec4 gBufferPosition = imageLoad(gBufferPositionImage, coordinate);
	uint triangleIndex = uint(gBufferPosition.a);

	if (triangleIndex == 0) {
		return;
	}

	uint seed = coordinate.y * arguments.atlasSize.x + coordinate.x;
	vec4 positionData = imageLoad(gBufferPositionImage, coordinate);
	vec4 smoothPositionData = imageLoad(gBufferSmoothPositionImage, coordinate);
	vec4 triangleNormalData = imageLoad(gBufferTriangleNormalImage, coordinate);
	vec4 vertexNormalData = imageLoad(gBufferVertexNormalImage, coordinate);

	vec3 position = positionData.rgb;
	vec3 smoothPosition = smoothPositionData.rgb;
	vec3 triangleNormal = triangleNormalData.rgb;
	float diagonalLuxelSize = triangleNormalData.a;
	vec3 vertexNormal = vertexNormalData.rgb;

	Hit hit;
	hit.position = position;
	hit.triangleNormal = triangleNormal;
	hit.materialIndex = uint(vertexNormalData.w);
	hit.vertexNormal = vertexNormal;

	vec3 radiance = vec3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	while (sampleIndex < SAMPLE_COUNT) {
		radiance += rgen(hit, seed);

		sampleIndex += 1;
	}

	vec3 light = radiance / SAMPLE_COUNT;
	light = pow(light, vec3(1.0 / 2.2));

	imageStore(intermediateLightmapAtlas, coordinate, vec4(light, 1.0));
}