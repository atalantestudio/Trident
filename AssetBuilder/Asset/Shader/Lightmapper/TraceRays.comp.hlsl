// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct TraceRaysArguments {
	uint2 atlasSize;
	uint luxelCount;
	uint triangleCount;
};

struct Luxel {
	uint triangleIndex;
	uint coordinate;
};

struct Vertex {
	float2 lightmapCoordinate;
	uint vertexIndex;
	uint normalIndex;
	uint textureCoordinateIndex;
	uint _;
};

struct Triangle {
	Vertex vertices[3];
	float3 normal;
};

struct Ray {
	float3 origin;
	float3 direction;
};

struct Hit {
	float distance;
	float3 position;
	float3 normal;
};

static const float FLOAT32_MAX = 4294967295.0;
static const uint SAMPLE_COUNT = 1024;
static const uint BOUNCE_COUNT = 4;
static const float3 SUN_DIRECTION = float3(-1.0, -1.0, -1.0);

StructuredBuffer<TraceRaysArguments> traceRaysArguments : register(t0);
StructuredBuffer<Luxel> luxels : register(t1);
StructuredBuffer<Vertex> indices : register(t2);
StructuredBuffer<float> vertexComponents : register(t3);
StructuredBuffer<float> normalComponents : register(t4);
RWTexture2D<float4> intermediateLightmapAtlas : register(u0);

float parallelogramArea(float2 a, float2 b) {
	return a.x * b.y - a.y * b.x;
}

float3 calculateBarycentricCoordinate(float2 a, float2 b, float2 c, float2 p) {
	float2 ab = b - a;
	float2 ac = c - a;
	float2 ap = p - a;
	float inverseArea = 1.0 / parallelogramArea(ab, ac);
	float v = parallelogramArea(ap, ac) * inverseArea;
	float w = parallelogramArea(ab, ap) * inverseArea;
	float u = 1.0 - v - w;

	return float3(u, v, w);
}

void pcgHash(inout uint seed) {
	uint state = seed * 747796405 + 2891336453;
	uint word = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;

	seed = (word >> 22) ^ word;
}

float randomFloat(inout uint seed) {
	pcgHash(seed);

	return seed / FLOAT32_MAX;
}

float3 randomUnitDirection(inout uint seed) {
	return normalize(float3(randomFloat(seed), randomFloat(seed), randomFloat(seed)) * 2.0 - 1.0);
}

Triangle loadTriangle(uint triangleIndex) {
	uint indexOffset = triangleIndex * 3;

	Triangle triangle_;
	triangle_.vertices[0] = indices[indexOffset + 0];
	triangle_.vertices[1] = indices[indexOffset + 1];
	triangle_.vertices[2] = indices[indexOffset + 2];

	uint vertexAOffset = triangle_.vertices[0].vertexIndex * 3;
	uint vertexBOffset = triangle_.vertices[1].vertexIndex * 3;
	uint vertexCOffset = triangle_.vertices[2].vertexIndex * 3;
	float3 a = float3(vertexComponents[vertexAOffset + 0], vertexComponents[vertexAOffset + 1], vertexComponents[vertexAOffset + 2]);
	float3 b = float3(vertexComponents[vertexBOffset + 0], vertexComponents[vertexBOffset + 1], vertexComponents[vertexBOffset + 2]);
	float3 c = float3(vertexComponents[vertexCOffset + 0], vertexComponents[vertexCOffset + 1], vertexComponents[vertexCOffset + 2]);

	triangle_.normal = normalize(cross(b - a, c - a));

	return triangle_;
}

bool rayTriangleIntersection(Ray ray, Triangle triangle_, inout float distance) {
	if (dot(ray.direction, triangle_.normal) > 1e-4) {
		return false;
	}

	uint vertexAOffset = triangle_.vertices[0].vertexIndex * 3;
	uint vertexBOffset = triangle_.vertices[1].vertexIndex * 3;
	uint vertexCOffset = triangle_.vertices[2].vertexIndex * 3;
	float3 a = float3(vertexComponents[vertexAOffset + 0], vertexComponents[vertexAOffset + 1], vertexComponents[vertexAOffset + 2]);
	float3 b = float3(vertexComponents[vertexBOffset + 0], vertexComponents[vertexBOffset + 1], vertexComponents[vertexBOffset + 2]);
	float3 c = float3(vertexComponents[vertexCOffset + 0], vertexComponents[vertexCOffset + 1], vertexComponents[vertexCOffset + 2]);
	float3 ab = b - a;
	float3 ac = c - a;
	float3 pvec = cross(ray.direction, ac);
	float d = dot(ab, pvec);

	if (abs(d) < 1e-4) {
		return false;
	}

	float invD = 1.0 / d;
	float3 tvec = ray.origin - a;
	float u = dot(tvec, pvec) * invD;

	if (u < -1e-4 || u > 1.0 + 1e-4) {
		return false;
	}

	float3 qvec = cross(tvec, ab);
	float v = dot(ray.direction, qvec) * invD;

	if (v < -1e-4 || u + v > 1.0 + 1e-4) {
		return false;
	}

	distance = dot(ac, qvec) * invD;

	return distance > 1e-4;
}

Hit rchit(Ray ray, Triangle triangle_, float distance) {
	Hit hit;
	hit.distance = distance;
	hit.position = ray.origin + ray.direction * distance;
	hit.normal = triangle_.normal;

	return hit;
}

Hit rmiss() {
	Hit hit;
	hit.distance = -1.0;

	return hit;
}

Hit rint(Ray ray) {
	float closestDistance = FLOAT32_MAX;
	uint intersectingTriangleIndex = traceRaysArguments[0].triangleCount;
	Triangle intersectingTriangle;
	uint triangleIndex = 0;

	while (triangleIndex < traceRaysArguments[0].triangleCount) {
		Triangle triangle_ = loadTriangle(triangleIndex);
		float distance;

		if (rayTriangleIntersection(ray, triangle_, distance)) {
			if (distance < closestDistance) {
				closestDistance = distance;
				intersectingTriangleIndex = triangleIndex;
				intersectingTriangle = triangle_;
			}
		}

		triangleIndex += 1;
	}

	if (intersectingTriangleIndex < traceRaysArguments[0].triangleCount) {
		return rchit(ray, intersectingTriangle, closestDistance);
	}

	return rmiss();
}

float3 rgen(Hit hit, inout uint seed) {
	Ray ray;

	Ray shadowRay;
	shadowRay.direction = normalize(-SUN_DIRECTION);

	Hit shadowHit;

	float3 radiance = float3(0.0, 0.0, 0.0);
	float3 throughput = float3(1.0, 1.0, 1.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		ray.origin = hit.position + hit.normal * 1e-4;
		ray.direction = normalize(hit.normal + randomUnitDirection(seed));

		hit = rint(ray);

		if (hit.distance < 0.0) {
			radiance += throughput;

			break;
		}

		shadowRay.origin = ray.origin;

		shadowHit = rint(shadowRay);

		radiance += throughput * (shadowHit.distance < 0.0);
		throughput *= 0.9;

		bounceIndex += 1;
	}

	return radiance;
}

[numthreads(16, 1, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint luxelIndex = globalInvocationIndex.x;

	if (luxelIndex >= traceRaysArguments[0].luxelCount) {
		return;
	}

	Luxel luxel = luxels[luxelIndex];

	uint2 coordinate = uint2(luxel.coordinate >> 16, luxel.coordinate & 0xffff);
	uint seed = coordinate.y * traceRaysArguments[0].atlasSize.x + coordinate.x;
	Triangle triangle_ = loadTriangle(luxel.triangleIndex);
	uint vertexAOffset = triangle_.vertices[0].vertexIndex * 3;
	uint vertexBOffset = triangle_.vertices[1].vertexIndex * 3;
	uint vertexCOffset = triangle_.vertices[2].vertexIndex * 3;
	float3 vertexA = float3(vertexComponents[vertexAOffset + 0], vertexComponents[vertexAOffset + 1], vertexComponents[vertexAOffset + 2]);
	float3 vertexB = float3(vertexComponents[vertexBOffset + 0], vertexComponents[vertexBOffset + 1], vertexComponents[vertexBOffset + 2]);
	float3 vertexC = float3(vertexComponents[vertexCOffset + 0], vertexComponents[vertexCOffset + 1], vertexComponents[vertexCOffset + 2]);
	float2 luxelCoordinate = (coordinate + float2(0.5, 0.5)) / traceRaysArguments[0].atlasSize;
	float3 luxelBarycentricCoordinate = calculateBarycentricCoordinate(triangle_.vertices[0].lightmapCoordinate, triangle_.vertices[1].lightmapCoordinate, triangle_.vertices[2].lightmapCoordinate, luxelCoordinate);
	float3 radiance = float3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	Hit hit;
	hit.position = vertexA * luxelBarycentricCoordinate.x + vertexB * luxelBarycentricCoordinate.y + vertexC * luxelBarycentricCoordinate.z;
	hit.normal = triangle_.normal;

	while (sampleIndex < SAMPLE_COUNT) {
		radiance += rgen(hit, seed);

		sampleIndex += 1;
	}

	float3 light = radiance / SAMPLE_COUNT;

	intermediateLightmapAtlas[coordinate] = float4(light, 1.0);
}