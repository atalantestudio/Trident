// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Lightmap.hlsl"

void traceSecondaryRay(inout uint seed) {
	const Arguments arguments = argumentBuffer[0];

	if (arguments.emissiveTriangleCount == 0) {
		return;
	}

	uint triangleIndex = randomScalar(seed) * arguments.emissiveTriangleCount;

	// TODO
	// 1. Load positions for A, B, C
	// 2. Generate random triangle barycentrics
	// 3. Interpolate to get final point
	// 4. Calculate PDF with triangle area

	// Can precompute:
	// - Positions A, B, C
	// - Triangle area
}

float3 traceRay(Hit primaryHit, inout uint seed) {
	const Arguments arguments = argumentBuffer[0];

	float3 light = float3(0.0, 0.0, 0.0);
	float3 throughput = float3(1.0, 1.0, 1.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		Material material = materialBuffer[primaryHit.materialIndex];

		// TODO
		// traceSecondaryRay(seed);

		RayDesc primaryRay;
		primaryRay.Origin = primaryHit.position + sign(primaryHit.vertexNormal) * abs(primaryHit.position * ADAPTIVE_BIAS);
		primaryRay.TMin = 0.001;
		primaryRay.Direction = normalize(primaryHit.vertexNormal + randomUnitDirection(seed));

		// TODO(asset-builder/lightmapper): Use the scene bounds.
		primaryRay.TMax = 2048.0;

		TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xff, 0, 0, 0, primaryRay, primaryHit);

		if (primaryHit.t < 0.0) {
			// light += arguments.skyColor * throughput;
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

	const Arguments arguments = argumentBuffer[0];

	float3 position = gBufferPosition.rgb;
	float4 gBufferTriangleNormal = gBufferTriangleNormalImage[coordinate];
	float3 triangleNormal = gBufferTriangleNormal.rgb;
	float diagonalLuxelSize = gBufferTriangleNormal.a;

	// TODO
	intermediateLightmapAtlas[coordinate] = float4(triangleNormal * 0.5 + 0.5, 1.0);
	return;

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