// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct TraceRaysArguments {
	uint lightmapPixelCount;
	uint triangleCount;
};

struct LightmapPixel {
	uint triangleIndex;
	uint coordinate;
};

struct Vertex {
	uint index;
	uint textureCoordinateIndex;
	uint lightmapCoordinateIndex;
};

struct Triangle {
	Vertex vertices[3];
	uint normalIndex;
};

struct Ray {
	float3 origin;
	float3 direction;
};

struct Hit {
	float t;
	float3 position;
	float3 normal;
};

static const float FLOAT32_MAX = 4294967295.0;
static const uint SAMPLE_COUNT = 1024;
static const uint BOUNCE_COUNT = 32;
static const uint2 ATLAS_SIZE = uint2(2048, 2048);

StructuredBuffer<TraceRaysArguments> traceRaysArguments : register(t0);
StructuredBuffer<LightmapPixel> lightmapPixels : register(t1);
StructuredBuffer<Triangle> triangles : register(t2);
StructuredBuffer<float> vertexComponents : register(t3);
StructuredBuffer<float> normalComponents : register(t4);
StructuredBuffer<float2> lightmapCoordinates : register(t5);
RWTexture2D<float4> lightmapAtlasImage : register(u0);

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

bool rayTriangleIntersection(Ray ray, Triangle triangle_, inout float t) {
	uint normalOffset = triangle_.normalIndex * 3;
	float3 normal = float3(
		normalComponents[normalOffset + 0],
		normalComponents[normalOffset + 1],
		normalComponents[normalOffset + 2]
	);

	if (dot(ray.direction, normal) > 0.0) {
		return false;
	}

	uint vertexAOffset = triangle_.vertices[0].index * 3;
	uint vertexBOffset = triangle_.vertices[1].index * 3;
	uint vertexCOffset = triangle_.vertices[2].index * 3;
	float3 a = float3(
		vertexComponents[vertexAOffset + 0],
		vertexComponents[vertexAOffset + 1],
		vertexComponents[vertexAOffset + 2]
	);
	float3 b = float3(
		vertexComponents[vertexBOffset + 0],
		vertexComponents[vertexBOffset + 1],
		vertexComponents[vertexBOffset + 2]
	);
	float3 c = float3(
		vertexComponents[vertexCOffset + 0],
		vertexComponents[vertexCOffset + 1],
		vertexComponents[vertexCOffset + 2]
	);

	float3 ab = b - a;
	float3 ac = c - a;
	float3 pvec = cross(ray.direction, ac);
	float d = dot(ab, pvec);

	if (abs(d) < 0.0001) {
		return false;
	}

	float invD = 1.0 / d;
	float3 tvec = ray.origin - a;
	float u = dot(tvec, pvec) * invD;

	if (u < 0.0 || u > 1.0) {
		return false;
	}

	float3 qvec = cross(tvec, ab);
	float v = dot(ray.direction, qvec) * invD;

	if (v < 0.0 || u + v > 1.0) {
		return false;
	}

	t = dot(ac, qvec) * invD;

	if (t < 0.0) {
		return false;
	}

	return true;
}

Hit rchit(Ray ray, uint triangleIndex, float t) {
	Triangle triangle_ = triangles[triangleIndex];
	uint normalOffset = triangle_.normalIndex * 3;
	float3 normal = float3(
		normalComponents[normalOffset + 0],
		normalComponents[normalOffset + 1],
		normalComponents[normalOffset + 2]
	);

	Hit hit;
	hit.t = t;
	hit.position = ray.origin;
	hit.normal = normal;

	return hit;
}

Hit rmiss() {
	Hit hit;
	hit.t = -1.0;

	return hit;
}

Hit rint(Ray ray) {
	float closestT = FLOAT32_MAX;
	uint intersectingTriangleIndex;
	uint triangleIndex = 0;

	while (triangleIndex < traceRaysArguments[0].triangleCount) {
		Triangle triangle_ = triangles[triangleIndex];
		float t;

		if (rayTriangleIntersection(ray, triangle_, t)) {
			if (t < closestT) {
				closestT = t;
				intersectingTriangleIndex = triangleIndex;
			}
		}

		triangleIndex += 1;
	}

	if (closestT < FLOAT32_MAX) {
		return rchit(ray, intersectingTriangleIndex, closestT);
	}

	return rmiss();
}

float3 rgen(Hit hit, inout uint seed) {
	Ray ray;
	float3 light = float3(0.0, 0.0, 0.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		ray.direction = normalize(hit.normal + randomUnitDirection(seed));

		// Smaller numbers like 0.0001 cause visual artifacts.
		ray.origin = hit.position + ray.direction * 0.001;

		hit = rint(ray);

		if (hit.t < 0.0) {
			light += float(BOUNCE_COUNT - bounceIndex) / BOUNCE_COUNT;

			break;
		}

		bounceIndex += 1;
	}

	return light;
}

[numthreads(16, 1, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint pixelIndex = globalInvocationIndex.x;

	if (pixelIndex >= traceRaysArguments[0].lightmapPixelCount) {
		return;
	}

	LightmapPixel pixel = lightmapPixels[pixelIndex];

	uint2 coordinate = uint2(pixel.coordinate >> 16, pixel.coordinate & 0xffff);
	uint seed = coordinate.y * ATLAS_SIZE.x + coordinate.x;

	Triangle triangle_ = triangles[pixel.triangleIndex];
	uint normalOffset = triangle_.normalIndex * 3;
	float3 normal = float3(
		normalComponents[normalOffset + 0],
		normalComponents[normalOffset + 1],
		normalComponents[normalOffset + 2]
	);
	uint vertexAOffset = triangle_.vertices[0].index * 3;
	uint vertexBOffset = triangle_.vertices[1].index * 3;
	uint vertexCOffset = triangle_.vertices[2].index * 3;
	float3 vertexA = float3(
		vertexComponents[vertexAOffset + 0],
		vertexComponents[vertexAOffset + 1],
		vertexComponents[vertexAOffset + 2]
	);
	float3 vertexB = float3(
		vertexComponents[vertexBOffset + 0],
		vertexComponents[vertexBOffset + 1],
		vertexComponents[vertexBOffset + 2]
	);
	float3 vertexC = float3(
		vertexComponents[vertexCOffset + 0],
		vertexComponents[vertexCOffset + 1],
		vertexComponents[vertexCOffset + 2]
	);
	float2 lightmapCoordinateA = lightmapCoordinates[triangle_.vertices[0].lightmapCoordinateIndex];
	float2 lightmapCoordinateB = lightmapCoordinates[triangle_.vertices[1].lightmapCoordinateIndex];
	float2 lightmapCoordinateC = lightmapCoordinates[triangle_.vertices[2].lightmapCoordinateIndex];
	float2 pixelCoordinate = (coordinate + float2(0.5, 0.5)) / ATLAS_SIZE;
	float3 pixelBarycentricCoordinate = calculateBarycentricCoordinate(lightmapCoordinateA, lightmapCoordinateB, lightmapCoordinateC, pixelCoordinate);

	Hit hit;
	hit.position = vertexA * pixelBarycentricCoordinate.x + vertexB * pixelBarycentricCoordinate.y + vertexC * pixelBarycentricCoordinate.z;
	hit.normal = normal;

	float3 light = float3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	while (sampleIndex < SAMPLE_COUNT) {
		light += rgen(hit, seed);

		sampleIndex += 1;
	}

	float3 averageLight = light / SAMPLE_COUNT;

	lightmapAtlasImage[coordinate] = float4(averageLight, 1.0);
}