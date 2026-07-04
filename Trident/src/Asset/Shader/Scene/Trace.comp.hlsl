// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 inverseProjection;
	float4x4 inverseView;
	float3 position;
	uint2 viewport;
	uint frameIndex;
	uint triangleCount;
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
static const uint RAY_BOUNCE_COUNT = 2;
static const float3 SKY_COLOR = uint3(130, 200, 229) / 255.0;

cbuffer Camera : register(b0) {
	Camera camera;
}

StructuredBuffer<Triangle> triangleBuffer : register(t0);
StructuredBuffer<float> vertexBuffer : register(t1);
StructuredBuffer<float> normalBuffer : register(t2);
StructuredBuffer<float> textureCoordinateBuffer : register(t3);
RWStructuredBuffer<float4> accumulateBuffer : register(u0);
RWTexture2D<float4> outputImage : register(u1);

float2 ndc(float2 coordinate) {
	float2 ndcCoordinate = coordinate / camera.viewport * 2.0 - 1.0;
	ndcCoordinate.y *= -1.0;

	return ndcCoordinate;
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
	float3 a = float3(
		vertexBuffer[triangle_.vertices[0].index * 3 + 0],
		vertexBuffer[triangle_.vertices[0].index * 3 + 1],
		vertexBuffer[triangle_.vertices[0].index * 3 + 2]
	);
	float3 b = float3(
		vertexBuffer[triangle_.vertices[1].index * 3 + 0],
		vertexBuffer[triangle_.vertices[1].index * 3 + 1],
		vertexBuffer[triangle_.vertices[1].index * 3 + 2]
	);
	float3 c = float3(
		vertexBuffer[triangle_.vertices[2].index * 3 + 0],
		vertexBuffer[triangle_.vertices[2].index * 3 + 1],
		vertexBuffer[triangle_.vertices[2].index * 3 + 2]
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
	Triangle triangle_ = triangleBuffer[triangleIndex];
	uint normalOffset = triangle_.normalIndex * 3;
	float3 normal = float3(
		normalBuffer[normalOffset + 0],
		normalBuffer[normalOffset + 1],
		normalBuffer[normalOffset + 2]
	);

	Hit hit;
	hit.t = t;
	hit.position = ray.origin + ray.direction * t;
	hit.normal = normal;

	if (dot(ray.direction, normal) > 0.0) {
		hit.normal = -normal;
	}

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

	while (triangleIndex < camera.triangleCount) {
		Triangle triangle_ = triangleBuffer[triangleIndex];
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

float4 rgen(uint2 coordinate, uint seed) {
	float2 ndcCoordinate = ndc(coordinate);
	float4 position = float4(mul(camera.inverseProjection, float4(ndcCoordinate, 0.0, 1.0)).xyz, 0.0);
	float3 light = float3(0.0, 0.0, 0.0);
	float3 throughput = float3(1.0, 1.0, 1.0);

	Ray ray;
	ray.origin = camera.position;
	ray.direction = mul(camera.inverseView, position).xyz;

	uint bounceIndex = 0;

	while (bounceIndex < RAY_BOUNCE_COUNT) {
		Hit hit = rint(ray);

		if (hit.t < 0.0) {
			light += SKY_COLOR * throughput;

			break;
		}

		static const float3 albedo = float3(1.0, 1.0, 1.0);
		static const float roughness = 1.0;

		throughput *= albedo;

		float3 diffuseDirection = normalize(hit.normal + randomUnitDirection(seed));
		float3 specularDirection = reflect(ray.direction, hit.normal);

		ray.origin = hit.position + hit.normal * 0.0001;
		ray.direction = lerp(specularDirection, diffuseDirection, roughness);

		bounceIndex += 1;
	}

	return float4(light, 1.0);
}

[numthreads(16, 16, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 coordinate = globalInvocationIndex.xy;

	if (any(coordinate >= camera.viewport)) {
		return;
	}

	uint coordinateIndex = coordinate.y * camera.viewport.x + coordinate.x;
	uint seed = coordinateIndex * camera.frameIndex;
	float4 sample_ = rgen(coordinate, seed);

	if (camera.frameIndex == 0) {
		accumulateBuffer[coordinateIndex] = sample_;

		outputImage[coordinate] = sample_;
	}
	else {
		accumulateBuffer[coordinateIndex] += sample_;

		outputImage[coordinate] = accumulateBuffer[coordinateIndex] / camera.frameIndex;
	}
}