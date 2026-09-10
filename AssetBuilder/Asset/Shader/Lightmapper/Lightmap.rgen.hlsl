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

float lengthSquared(float3 a) {
	return dot(a, a);
}

float parallelogramArea(float2 a, float2 b) {
	return a.x * b.y - a.y * b.x;
}

float balanceHeuristic(float pdf1, float pdf2) {
	return pdf1 / (pdf1 + pdf2);
}

void initializeSecondaryRay(RayDesc primaryRay, inout uint triangleIndex, inout float3 p, inout float pdf, inout uint seed) {
	uint emissiveTriangleIndexOffset = uint(randomScalar(seed) * arguments.emissiveTriangleCount);
	uint emissiveTriangleIndex = emissiveTriangleIndexBuffer[emissiveTriangleIndexOffset];

	Triangle triangle_ = triangleBuffer[emissiveTriangleIndex];

	triangleIndex = triangle_.index;

	float2 a = lightmapCoordinateBuffer[triangle_.lightmapCoordinateIndexA];
	float2 b = lightmapCoordinateBuffer[triangle_.lightmapCoordinateIndexB];
	float2 c = lightmapCoordinateBuffer[triangle_.lightmapCoordinateIndexC];

	float abc = length(parallelogramArea(b - a, c - a)) * 0.5;

	// TODO(asset-builder/lightmapper/is)
	// pdf = 1.0 / (abc * arguments.emissiveTriangleCount);
	pdf = (abc * arguments.atlasSize.x) / arguments.emissiveTriangleCount;

	float u = randomScalar(seed);
	float v = randomScalar(seed);

	if (u + v > 1.0) {
		u = 1.0 - u;
		v = 1.0 - v;
	}

	float w = 1.0 - u - v;
	uint2 coordinate = (a * u + b * v + c * w) * arguments.atlasSize;

	p = gBufferPositionImage[coordinate].rgb;
}

float3 traceRay(Hit primaryHit, inout uint seed) {
	const float pdfDiffuse = 1.0 / (4.0 * PI);

	float3 light = float3(0.0, 0.0, 0.0);
	float3 throughput = float3(1.0, 1.0, 1.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		Material material = materialBuffer[primaryHit.materialIndex];

		float3 directLight = float3(0.0, 0.0, 0.0);

		RayDesc primaryRay;
		primaryRay.Origin = primaryHit.position + sign(primaryHit.vertexNormal) * abs(primaryHit.position * ADAPTIVE_BIAS);
		primaryRay.TMin = 0.001;
		primaryRay.Direction = normalize(primaryHit.vertexNormal + randomUnitDirection(seed));

		// TODO(asset-builder/lightmapper): Use the scene bounds.
		primaryRay.TMax = 2048.0;

		if (arguments.emissiveTriangleCount > 0) {
			uint triangleIndex;
			float3 p;
			float pdf;

			initializeSecondaryRay(primaryRay, triangleIndex, p, pdf, seed);

			RayDesc secondaryRay = primaryRay;
			secondaryRay.Direction = normalize(p - primaryRay.Origin);

			if (dot(primaryHit.vertexNormal, secondaryRay.Direction) > 1e-4) {
				Hit secondaryHit;

				TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xff, 0, 0, 0, secondaryRay, secondaryHit);

				if (secondaryHit.triangleIndex == triangleIndex) {
					Material material = materialBuffer[secondaryHit.materialIndex];

					float angleA = dot(secondaryRay.Direction, primaryHit.vertexNormal);
					float angleB = dot(secondaryRay.Direction, secondaryHit.vertexNormal);
					float G = 1.0 / lengthSquared(p - primaryRay.Origin) * angleA * max(0.001, abs(angleB));
					float w = balanceHeuristic(pdf, pdfDiffuse * G);

					directLight += material.emissiveColor * material.emissiveIntensity * G / pdf * w;
				}
			}
		}

		TraceRay(accelerationStructure, RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xff, 0, 0, 0, primaryRay, primaryHit);

		if (primaryHit.t < 0.0) {
			light += environmentMapTexture.SampleLevel(environmentMapSamplerState, primaryRay.Direction, 0.0).rgb * throughput;

			break;
		}

		float3 brdf = material.albedo / PI;

		light += directLight * throughput * brdf;
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