// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

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

/* void traceSecondaryRay(inout uint seed) {
	if (arguments.emissiveTriangleCount == 0) {
		return;
	}

	uint triangleIndex = randomScalar(seed) * arguments.emissiveTriangleCount;

	Triangle triangle_ = emissiveTriangleBuffer[triangleIndex];
	Instance instance = instanceBuffer[triangle_.instanceIndex];

	uint indexA = indexBuffer[instance.geometryTriangleOffset * 3 + triangle_.indexOffset + 0];
	uint indexB = indexBuffer[instance.geometryTriangleOffset * 3 + triangle_.indexOffset + 1];
	uint indexC = indexBuffer[instance.geometryTriangleOffset * 3 + triangle_.indexOffset + 2];

	float3 a = mul(float4(vertexBuffer[indexA].position, 1.0), instance.transform);
	float3 b = mul(float4(vertexBuffer[indexB].position, 1.0), instance.transform);
	float3 c = mul(float4(vertexBuffer[indexC].position, 1.0), instance.transform);

	float abc = length(cross(b - a, c - a)) * 0.5;
	float pdf = 1.0 / (abc * arguments.emissiveTriangleCount);

	float u = randomScalar(seed);
	float v = randomScalar(seed);

	if (u + v > 1.0) {
		u = 1.0 - u;
		v = 1.0 - v;
	}

	float w = 1.0 - u - v;
	float3 p = a * u + b * v + c * w;

	// TODO
*/

float3 traceRay(Hit primaryHit, inout uint seed) {
	float3 light = float3(0.0, 0.0, 0.0);
	float3 throughput = float3(1.0, 1.0, 1.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		Material material = materialBuffer[primaryHit.materialIndex];

		// TODO(asset-builder/lightmapper): Trace secondary ray.
		// traceSecondaryRay(seed);

		RayDesc primaryRay;
		primaryRay.Origin = primaryHit.position + sign(primaryHit.vertexNormal) * abs(primaryHit.position * ADAPTIVE_BIAS);
		primaryRay.TMin = 0.001;
		primaryRay.Direction = normalize(primaryHit.vertexNormal + randomUnitDirection(seed));

		// TODO(asset-builder/lightmapper): Use the scene bounds.
		primaryRay.TMax = 2048.0;

		TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xff, 0, 0, 0, primaryRay, primaryHit);

		if (primaryHit.t < 0.0) {
			light += environmentMapTexture.SampleLevel(environmentMapSamplerState, primaryRay.Direction, 0.0).rgb * throughput;

			break;
		}

		throughput *= material.albedo;

		bounceIndex += 1;
	}

	return light;
}

bool adjustSamplePosition(inout float3 position, float3 triangleNormal, float diagonalLuxelSize) {
	const float bias = 1e-4;

	float3 a = normalize(cross(position, triangleNormal));
	float3 b = normalize(cross(a, triangleNormal));

	RayDesc ray;
	ray.Origin = position + sign(triangleNormal) * abs(position * bias);
	ray.TMin = 0.001;
	ray.Direction = a;
	ray.TMax = diagonalLuxelSize * 0.5;

	Hit hit;

	TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_FRONT_FACING_TRIANGLES, 0xff, 0, 0, 0, ray, hit);

	if (hit.t > 0.0) {
		position = hit.position + sign(hit.triangleNormal) * abs(hit.position * bias);

		return true;
	}

	ray.Direction = b;

	TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_FRONT_FACING_TRIANGLES, 0xff, 0, 0, 0, ray, hit);

	if (hit.t > 0.0) {
		position = hit.position + sign(hit.triangleNormal) * abs(hit.position * bias);

		return true;
	}

	ray.Direction = -a;

	TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_FRONT_FACING_TRIANGLES, 0xff, 0, 0, 0, ray, hit);

	if (hit.t > 0.0) {
		position = hit.position + sign(hit.triangleNormal) * abs(hit.position * bias);

		return true;
	}

	ray.Direction = -b;

	TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_FRONT_FACING_TRIANGLES, 0xff, 0, 0, 0, ray, hit);

	if (hit.t > 0.0) {
		position = hit.position + sign(hit.triangleNormal) * abs(hit.position * bias);

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