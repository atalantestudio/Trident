// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct TraceRaysArguments {
	uint2 atlasSize;
	uint triangleCount;
	uint _0;
	float3 skyColor;
	uint _1;
	float3 sunDirection;
	uint _2;
	float3 sunColor;
	float sunIntensity;
};

struct Vertex {
	float3 position;
	float3 normal;
	float3 tangent;
	float2 lightmapCoordinate;
};

struct Triangle {
	Vertex vertices[3];
};

struct Ray {
	float3 origin;
	float3 direction;
};

struct Hit {
	float3 position;
	float distance;
	float3 triangleNormal;
	float3 vertexNormal;
};

static const float FLOAT32_MAX = 4294967295.0;
static const uint SAMPLE_COUNT = 1024;
static const uint BOUNCE_COUNT = 4;
static const float ADAPTIVE_BIAS = 0.0000002;
static const bool FRONT_FACE = true;
static const bool BACK_FACE = false;

StructuredBuffer<TraceRaysArguments> traceRaysArguments : register(t0);
StructuredBuffer<Vertex> vertexBuffer : register(t1);
StructuredBuffer<uint> indexBuffer : register(t2);
Texture2D<float4> gBufferPositionImage : register(t3);
Texture2D<float4> gBufferTriangleNormalImage : register(t4);
Texture2D<float4> gBufferVertexNormalImage : register(t5);
RWTexture2D<float4> intermediateLightmapAtlas : register(u0);

Triangle loadTriangle(uint triangleIndex) {
	uint indexOffset = triangleIndex * 3;
	uint indexA = indexBuffer[indexOffset + 0];
	uint indexB = indexBuffer[indexOffset + 1];
	uint indexC = indexBuffer[indexOffset + 2];

	Triangle triangle_;
	triangle_.vertices[0] = vertexBuffer[indexA];
	triangle_.vertices[1] = vertexBuffer[indexB];
	triangle_.vertices[2] = vertexBuffer[indexC];

	return triangle_;
}

void pcgHash(inout uint seed) {
	uint state = seed * 747796405 + 2891336453;
	uint word = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;

	seed = (word >> 22) ^ word;
}

float randomScalar(inout uint seed) {
	pcgHash(seed);

	return seed / FLOAT32_MAX;
}

float3 randomUnitDirection(inout uint seed) {
	return normalize(float3(randomScalar(seed), randomScalar(seed), randomScalar(seed)) * 2.0 - 1.0);
}

bool rayTriangleIntersection(Ray ray, Triangle triangle_, bool face, inout float distance, inout float3 barycentricCoordinate) {
	const float E = 1e-8;
	const float E2 = 1e-4;

	float3 a = triangle_.vertices[0].position;
	float3 b = triangle_.vertices[1].position;
	float3 c = triangle_.vertices[2].position;
	float3 ab = b - a;
	float3 ac = c - a;
	float3 p = cross(ray.direction, ac);
	float d = dot(ab, p);
	bool test = d >= E;

	if (face == BACK_FACE) {
		test = !test;
	}

	if (test) {
		float inverseD = 1.0 / d;
		float3 t = ray.origin - a;
		float w = dot(t, p) * inverseD;

		if (w < -E2 || w > 1.0 + E2) {
			test = false;
		}
		else {
			float3 q = cross(t, ab);
			float v = dot(ray.direction, q) * inverseD;

			if (v < -E2 || v + w > 1.0 + E2) {
				test = false;
			}
			else {
				float u = 1.0 - v - w;

				barycentricCoordinate = float3(u, v, w);

				distance = dot(ac, q) * inverseD;

				test = distance > 0.0;
			}
		}
	}

	return test;
}

Hit rchit(Ray ray, Triangle triangle_, float distance, float3 barycentricCoordinate) {
	float2 lightmapCoordinate = (
		triangle_.vertices[0].lightmapCoordinate * barycentricCoordinate.x +
		triangle_.vertices[1].lightmapCoordinate * barycentricCoordinate.y +
		triangle_.vertices[2].lightmapCoordinate * barycentricCoordinate.z
	) * traceRaysArguments[0].atlasSize;

	float4 triangleNormalData = gBufferTriangleNormalImage[lightmapCoordinate];
	float4 vertexNormalData = gBufferVertexNormalImage[lightmapCoordinate];

	Hit hit;
	hit.position = ray.origin + ray.direction * distance;
	hit.distance = distance;
	hit.triangleNormal = triangleNormalData.rgb;
	hit.vertexNormal = vertexNormalData.rgb;

	return hit;
}

Hit rmiss() {
	Hit hit;
	hit.distance = -1.0;

	return hit;
}

Hit rint(Ray ray, bool face) {
	float closestDistance = FLOAT32_MAX;
	uint intersectingTriangleIndex = traceRaysArguments[0].triangleCount;
	Triangle intersectingTriangle;
	float3 intersectingBarycentricCoordinate;
	uint triangleIndex = 0;

	while (triangleIndex < traceRaysArguments[0].triangleCount) {
		Triangle triangle_ = loadTriangle(triangleIndex);
		float distance = 0.0;
		float3 barycentricCoordinate = float3(0.0, 0.0, 0.0);

		if (rayTriangleIntersection(ray, triangle_, face, distance, barycentricCoordinate)) {
			if (distance < closestDistance) {
				closestDistance = distance;
				intersectingTriangleIndex = triangleIndex;
				intersectingTriangle = triangle_;
				intersectingBarycentricCoordinate = barycentricCoordinate;
			}
		}

		triangleIndex += 1;
	}

	Hit hit;

	if (intersectingTriangleIndex >= traceRaysArguments[0].triangleCount) {
		hit = rmiss();
	}
	else {
		hit = rchit(ray, intersectingTriangle, closestDistance, intersectingBarycentricCoordinate);
	}

	return hit;
}

float3 rgen(Hit primaryHit, inout uint seed) {
	// TODO(asset-builder/lightmapper): Use the albedo of the triangle being hit.
	float3 albedo = traceRaysArguments[0].skyColor;

	float3 radiance = float3(0.0, 0.0, 0.0);
	float3 throughput = float3(1.0, 1.0, 1.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		Ray primaryRay;
		primaryRay.origin = primaryHit.position + sign(primaryHit.vertexNormal) * abs(primaryHit.position * ADAPTIVE_BIAS);
		primaryRay.direction = normalize(primaryHit.vertexNormal + randomUnitDirection(seed));

		primaryHit = rint(primaryRay, FRONT_FACE);

		Ray secondaryRay;
		secondaryRay.origin = primaryRay.origin;
		secondaryRay.direction = normalize(traceRaysArguments[0].sunDirection);

		Hit secondaryHit = rint(secondaryRay, FRONT_FACE);

		radiance += throughput * (secondaryHit.distance < 0.0) * traceRaysArguments[0].sunColor * traceRaysArguments[0].sunIntensity;
		throughput *= albedo;

		if (primaryHit.distance < 0.0) {
			radiance += throughput * traceRaysArguments[0].skyColor;

			break;
		}

		bounceIndex += 1;
	}

	return radiance;
}

float3 adjustSamplePosition(float3 position, float3 triangleNormal, float diagonalLuxelSize) {
	const float bias = 1e-4;

	float3 d0 = normalize(cross(position, triangleNormal));
	float3 d1 = normalize(cross(d0, triangleNormal));
	float rayLength = diagonalLuxelSize * 0.5;

	Ray ray;
	ray.origin = position + triangleNormal * bias;
	ray.direction = d0;

	Hit hit = rint(ray, BACK_FACE);

	if (hit.distance > 0.0 && hit.distance < rayLength) {
		return hit.position + hit.triangleNormal * bias;
	}

	ray.direction = d1;

	hit = rint(ray, BACK_FACE);

	if (hit.distance > 0.0 && hit.distance < rayLength) {
		return hit.position + hit.triangleNormal * bias;
	}

	ray.direction = -d0;
	hit = rint(ray, BACK_FACE);

	if (hit.distance > 0.0 && hit.distance < rayLength) {
		return hit.position + hit.triangleNormal * bias;
	}

	ray.direction = -d1;
	hit = rint(ray, BACK_FACE);

	if (hit.distance > 0.0 && hit.distance < rayLength) {
		return hit.position + hit.triangleNormal * bias;
	}

	return position;
}

[numthreads(16, 16, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 coordinate = globalInvocationIndex.xy;

	if (coordinate.x >= traceRaysArguments[0].atlasSize.x || coordinate.y >= traceRaysArguments[0].atlasSize.y) {
		return;
	}

	uint seed = coordinate.y * traceRaysArguments[0].atlasSize.x + coordinate.x;
	float4 positionData = gBufferPositionImage[coordinate];
	float4 triangleNormalData = gBufferTriangleNormalImage[coordinate];
	float4 vertexNormalData = gBufferVertexNormalImage[coordinate];

	float3 position = positionData.rgb;
	uint triangleIndex = uint(positionData.a);
	float3 triangleNormal = triangleNormalData.rgb;
	float diagonalLuxelSize = triangleNormalData.a;
	float3 vertexNormal = vertexNormalData.rgb;

	if (triangleIndex == 0) {
		return;
	}

	Hit hit;
	hit.position = adjustSamplePosition(position, triangleNormal, diagonalLuxelSize);
	hit.triangleNormal = triangleNormal;
	hit.vertexNormal = vertexNormal;

	float3 radiance = float3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	while (sampleIndex < SAMPLE_COUNT) {
		radiance += rgen(hit, seed);

		sampleIndex += 1;
	}

	float3 light = radiance / SAMPLE_COUNT;

	intermediateLightmapAtlas[coordinate] = float4(light, 1.0);
}