// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#define CUSTOM_SCENE_DDGI_REF_0

#include "Lightmap.hlsl"

void pcgHash(inout uint seed) {
	uint state = seed * 747796405 + 2891336453;
	uint word = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;

	seed = (word >> 22) ^ word;
}

float randomScalar(inout uint seed) {
	pcgHash(seed);

	return seed / FLOAT_MAX;
}

float3 randomUnitDirection(inout uint seed) {
	return normalize(float3(randomScalar(seed), randomScalar(seed), randomScalar(seed)) * 2.0 - 1.0);
}

float3 sampleDirectLight(RayDesc primaryRay, Hit primaryHit, inout uint seed) {
	const float pdfDiffuse = 1.0 / (4.0 * PI);

#if defined(CUSTOM_SCENE_DDGI_REF_0)
	float3 p = float3(0.0, 64.0, 164.0);
	float3 op = p - primaryRay.Origin;
	float pdfLight = 1.0;

	RayDesc secondaryRay = primaryRay;
	secondaryRay.Direction = normalize(op);

	if (dot(primaryHit.vertexNormal, secondaryRay.Direction) < 0.0) {
		return float3(0.0, 0.0, 0.0);
	}

	Hit secondaryHit;

	TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xff, 0, 0, 0, secondaryRay, secondaryHit);

	if (secondaryHit.t < length(op)) {
		return float3(0.0, 0.0, 0.0);
	}

	float angleA = dot(secondaryRay.Direction, primaryHit.vertexNormal);
	float angleB = -1.0;
	float G = 1.0 / dot(op, op) * angleA * max(0.001, abs(angleB));
	float h = pdfLight / (pdfLight + pdfDiffuse * G);

	return 256.0 * G / pdfLight * h;
#else
	if (arguments.emissiveTriangleCount == 0) {
		return float3(0.0, 0.0, 0.0);
	}

	uint emissiveTriangleIndexIndex = uint(randomScalar(seed) * arguments.emissiveTriangleCount);
	uint emissiveTriangleIndex = emissiveTriangleIndexBuffer[emissiveTriangleIndexIndex];

	Triangle triangle_ = triangleBuffer[emissiveTriangleIndex];

	float2 a = lightmapCoordinateBuffer[triangle_.lightmapCoordinateIndexA];
	float2 b = lightmapCoordinateBuffer[triangle_.lightmapCoordinateIndexB];
	float2 c = lightmapCoordinateBuffer[triangle_.lightmapCoordinateIndexC];

	float u = randomScalar(seed);
	float v = randomScalar(seed);

	if (u + v > 1.0) {
		u = 1.0 - u;
		v = 1.0 - v;
	}

	float w = 1.0 - u - v;
	uint2 coordinate = (a * u + b * v + c * w) * arguments.atlasSize;

	float3 p = gBufferPositionImage[coordinate].rgb;
	float3 op = p - primaryRay.Origin;
	float area = gBufferSmoothPositionImage[coordinate].a;
	float pdfLight = 1.0 / (area * arguments.emissiveTriangleCount);

	RayDesc secondaryRay = primaryRay;
	secondaryRay.Direction = normalize(op);

	if (dot(primaryHit.vertexNormal, secondaryRay.Direction) < 0.0) {
		return float3(0.0, 0.0, 0.0);
	}

	Hit secondaryHit;

	TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xff, 0, 0, 0, secondaryRay, secondaryHit);

	if (secondaryHit.triangleIndex != triangle_.index) {
		return float3(0.0, 0.0, 0.0);
	}

	Material material = materialBuffer[secondaryHit.materialIndex];

	float angleA = dot(secondaryRay.Direction, primaryHit.vertexNormal);
	float angleB = dot(secondaryRay.Direction, secondaryHit.vertexNormal);
	float G = 1.0 / dot(op, op) * angleA * max(0.001, abs(angleB));
	float h = pdfLight / (pdfLight + pdfDiffuse * G);

	return material.emissiveColor * material.emissiveIntensity * G / pdfLight * h;
#endif
}

float3 traceRay(Hit primaryHit, inout uint seed) {
	float3 light = float3(0.0, 0.0, 0.0);
	float3 throughput = float3(1.0, 1.0, 1.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		Material material = materialBuffer[primaryHit.materialIndex];

		throughput *= material.albedo;

		RayDesc primaryRay;
		primaryRay.Origin = primaryHit.position + sign(primaryHit.vertexNormal) * abs(primaryHit.position * ADAPTIVE_BIAS);
		primaryRay.TMin = 0.0;
		primaryRay.Direction = normalize(primaryHit.vertexNormal + randomUnitDirection(seed));

		// TODO(asset-builder/lightmapper): Use the scene bounds.
		primaryRay.TMax = 2048.0;

		float3 directLight = sampleDirectLight(primaryRay, primaryHit, seed);
		float3 brdf = material.albedo / PI;

		light += directLight * throughput * brdf;

		TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xff, 0, 0, 0, primaryRay, primaryHit);

		if (primaryHit.t < 0.0) {
			light += environmentMapTexture.SampleLevel(environmentMapSamplerState, primaryRay.Direction, 0.0).rgb * throughput;

			break;
		}

		bounceIndex += 1;
	}

	return light;
}

bool adjustSamplePosition(inout float3 position, float3 triangleNormal, float diagonalLuxelSize) {
	float3 a = normalize(cross(position, triangleNormal));
	float3 b = normalize(cross(a, triangleNormal));

	RayDesc ray;
	ray.Origin = position + sign(triangleNormal) * abs(position * ADAPTIVE_BIAS);
	ray.TMin = 0.0;
	ray.Direction = a;
	ray.TMax = diagonalLuxelSize * 0.5;

	Hit hit;

	TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_FRONT_FACING_TRIANGLES, 0xff, 0, 0, 0, ray, hit);

	if (hit.t > 0.0) {
		position = hit.position + sign(hit.triangleNormal) * abs(hit.position * ADAPTIVE_BIAS);

		return true;
	}

	ray.Direction = b;

	TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_FRONT_FACING_TRIANGLES, 0xff, 0, 0, 0, ray, hit);

	if (hit.t > 0.0) {
		position = hit.position + sign(hit.triangleNormal) * abs(hit.position * ADAPTIVE_BIAS);

		return true;
	}

	ray.Direction = -a;

	TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_FRONT_FACING_TRIANGLES, 0xff, 0, 0, 0, ray, hit);

	if (hit.t > 0.0) {
		position = hit.position + sign(hit.triangleNormal) * abs(hit.position * ADAPTIVE_BIAS);

		return true;
	}

	ray.Direction = -b;

	TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_FRONT_FACING_TRIANGLES, 0xff, 0, 0, 0, ray, hit);

	if (hit.t > 0.0) {
		position = hit.position + sign(hit.triangleNormal) * abs(hit.position * ADAPTIVE_BIAS);

		return true;
	}

	return false;
}

[shader("raygeneration")]
void main() {
	uint2 coordinate = DispatchRaysIndex().xy;

	float4 gBufferPosition = gBufferPositionImage[coordinate];
	uint triangleIndex = gBufferPosition.a;

	if (triangleIndex == 0) {
		return;
	}

	float3 position = gBufferPosition.rgb;
	float4 gBufferTriangleNormal = gBufferTriangleNormalImage[coordinate];
	float3 triangleNormal = gBufferTriangleNormal.rgb;
	float diagonalLuxelSize = gBufferTriangleNormal.a;

	adjustSamplePosition(position, triangleNormal, diagonalLuxelSize);

	float4 gBufferSmoothPosition = gBufferSmoothPositionImage[coordinate];
	float3 smoothPosition = gBufferSmoothPosition.rgb;
	float4 gBufferVertexNormal = gBufferVertexNormalImage[coordinate];
	float3 vertexNormal = gBufferVertexNormal.rgb;
	uint materialIndex = gBufferVertexNormal.a;

	Material material = materialBuffer[materialIndex];

	if (material.emissiveIntensity > 0.0) {
		intermediateLightmapAtlas[coordinate] = float4(material.emissiveColor * material.emissiveIntensity, 1.0);

		return;
	}

	uint seed = coordinate.y * arguments.atlasSize.x + coordinate.x;

	Hit hit;
	hit.position = position;
	hit.triangleNormal = triangleNormal;
	hit.materialIndex = materialIndex;
	hit.vertexNormal = vertexNormal;

	float3 light = float3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	while (sampleIndex < SAMPLE_COUNT) {
		light += traceRay(hit, seed);

		sampleIndex += 1;
	}

	light /= SAMPLE_COUNT;
	light = pow(light, 1.0 / 2.2);

	intermediateLightmapAtlas[coordinate] = float4(light, 1.0);
}