#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

struct Vertex {
	vec2 lightmapCoordinate;
	uint vertexIndex;
	uint normalIndex;
	uint textureCoordinateIndex;
	uint _;
};

struct TriangleVertex {
	vec2 lightmapCoordinate;
	vec3 position;
};

struct Triangle {
	TriangleVertex vertices[3];
};

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Hit {
	vec3 position;
	float distance;
	vec3 normal;
	uint triangleIndex;
};

const float FLOAT32_MAX = 4294967295.0;
const uint SAMPLE_COUNT = 1024;
const uint BOUNCE_COUNT = 4;
const vec3 SUN_DIRECTION = vec3(1.0, 1.0, 1.0);
const vec3 SKY_COLOR = vec3(1.0, 1.0, 1.0);

layout(set = 0, binding = 0) buffer readonly TraceRaysArgumentBuffer {
	uvec2 atlasSize;
	uint triangleCount;

	// TODO(asset-builder/lightmapper): Move options such as sun direction and sky color here.
};

layout(set = 0, binding = 1) buffer readonly IndexBuffer {
	Vertex indices[];
};

layout(set = 0, binding = 2) buffer readonly VertexBuffer {
	float vertexBuffer[];
};

layout(set = 0, binding = 3, rgba32f) uniform readonly image2D gBufferPositionImage;

layout(set = 0, binding = 4, rgba32f) uniform readonly image2D gBufferTriangleNormalImage;

layout(set = 0, binding = 5, rgba32f) uniform readonly image2D gBufferVertexNormalImage;

layout(set = 0, binding = 6, rgba8) uniform writeonly image2D intermediateLightmapAtlas;

Triangle loadTriangle(uint triangleIndex) {
	uint indexOffset = triangleIndex * 3;

	Vertex indexA = indices[indexOffset + 0];
	Vertex indexB = indices[indexOffset + 1];
	Vertex indexC = indices[indexOffset + 2];

	uint vertexOffsetA = indexA.vertexIndex * 3;
	uint vertexOffsetB = indexB.vertexIndex * 3;
	uint vertexOffsetC = indexC.vertexIndex * 3;

	Triangle triangle_;
	triangle_.vertices[0].lightmapCoordinate = indexA.lightmapCoordinate;
	triangle_.vertices[0].position = vec3(vertexBuffer[vertexOffsetA + 0], vertexBuffer[vertexOffsetA + 1], vertexBuffer[vertexOffsetA + 2]);
	triangle_.vertices[1].lightmapCoordinate = indexB.lightmapCoordinate;
	triangle_.vertices[1].position = vec3(vertexBuffer[vertexOffsetB + 0], vertexBuffer[vertexOffsetB + 1], vertexBuffer[vertexOffsetB + 2]);
	triangle_.vertices[2].lightmapCoordinate = indexC.lightmapCoordinate;
	triangle_.vertices[2].position = vec3(vertexBuffer[vertexOffsetC + 0], vertexBuffer[vertexOffsetC + 1], vertexBuffer[vertexOffsetC + 2]);

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

vec3 randomUnitDirection(inout uint seed) {
	return normalize(vec3(randomScalar(seed), randomScalar(seed), randomScalar(seed)) * 2.0 - 1.0);
}

bool rayTriangleIntersection(Ray ray, Triangle triangle_, inout float distance, inout vec3 barycentricCoordinate) {
	const float E = 1e-8;
	const float E2 = 1e-4;

	vec3 a = triangle_.vertices[0].position;
	vec3 b = triangle_.vertices[1].position;
	vec3 c = triangle_.vertices[2].position;
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 p = cross(ray.direction, ac);
	float d = dot(ab, p);

	if (d < E) {
		return false;
	}

	if (abs(d) < E) {
		return false;
	}

	float inverseD = 1.0 / d;
	vec3 t = ray.origin - a;
	float w = dot(t, p) * inverseD;

	if (w < -E2 || w > 1.0 + E2) {
		return false;
	}

	vec3 q = cross(t, ab);
	float v = dot(ray.direction, q) * inverseD;

	if (v < -E2 || v + w > 1.0 + E2) {
		return false;
	}

	float u = 1.0 - v - w;

	barycentricCoordinate = vec3(u, v, w);

	distance = dot(ac, q) * inverseD;

	return distance > 0.0;
}

Hit rchit(Ray ray, Triangle triangle_, uint triangleIndex, float distance, vec3 barycentricCoordinate) {
	vec2 lightmapCoordinate = (
		triangle_.vertices[0].lightmapCoordinate * barycentricCoordinate.x +
		triangle_.vertices[1].lightmapCoordinate * barycentricCoordinate.y +
		triangle_.vertices[2].lightmapCoordinate * barycentricCoordinate.z
	);

	vec4 vertexNormalData = imageLoad(gBufferVertexNormalImage, ivec2(lightmapCoordinate * atlasSize));

	Hit hit;
	hit.position = ray.origin + ray.direction * distance;
	hit.distance = distance;
	hit.normal = vertexNormalData.rgb;
	hit.triangleIndex = triangleIndex;

	return hit;
}

Hit rmiss() {
	Hit hit;
	hit.distance = -1.0;

	return hit;
}

Hit rint(uint originTriangleIndex, Ray ray) {
	float closestDistance = FLOAT32_MAX;
	uint intersectingTriangleIndex = triangleCount;
	Triangle intersectingTriangle;
	vec3 intersectingBarycentricCoordinate;
	uint triangleIndex = 0;

	while (triangleIndex < triangleCount) {
		/* if (triangleIndex == originTriangleIndex) {
			triangleIndex += 1;

			continue;
		} */

		Triangle triangle_ = loadTriangle(triangleIndex);
		float distance;
		vec3 barycentricCoordinate;

		if (rayTriangleIntersection(ray, triangle_, distance, barycentricCoordinate)) {
			if (distance < closestDistance) {
				closestDistance = distance;
				intersectingTriangleIndex = triangleIndex;
				intersectingTriangle = triangle_;
				intersectingBarycentricCoordinate = barycentricCoordinate;
			}
		}

		triangleIndex += 1;
	}

	if (intersectingTriangleIndex < triangleCount) {
		return rchit(ray, intersectingTriangle, intersectingTriangleIndex, closestDistance, intersectingBarycentricCoordinate);
	}

	return rmiss();
}

vec3 rgen(Hit hit, inout uint seed) {
	Ray shadowRay;
	shadowRay.direction = normalize(SUN_DIRECTION);

	vec3 radiance = vec3(0.0, 0.0, 0.0);
	vec3 throughput = vec3(1.0, 1.0, 1.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		uint triangleIndex = hit.triangleIndex;

		Ray ray;
		ray.origin = hit.position + hit.normal * 0.0001;
		ray.direction = normalize(hit.normal + randomUnitDirection(seed));

		hit = rint(triangleIndex, ray);

		if (hit.distance < 0.0) {
			radiance += throughput * SKY_COLOR;

			break;
		}

		shadowRay.origin = ray.origin;

		Hit shadowHit = rint(triangleIndex, shadowRay);

		radiance += throughput * float(shadowHit.distance < 0.0);
		throughput *= 0.9;

		bounceIndex += 1;
	}

	return radiance;
}

void main() {
	ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);

	if (coordinate.x >= atlasSize.x || coordinate.y >= atlasSize.y) {
		return;
	}

	uint seed = coordinate.y * atlasSize.x + coordinate.x;
	vec4 positionData = imageLoad(gBufferPositionImage, coordinate);
	vec4 vertexNormalData = imageLoad(gBufferVertexNormalImage, coordinate);

	vec3 position = positionData.rgb;
	uint triangleIndex = uint(positionData.a);
	vec3 vertexNormal = vertexNormalData.rgb;

	if (triangleIndex == 0) {
		return;
	}

	triangleIndex -= 1;

	Hit hit;
	hit.position = position;
	hit.normal = vertexNormal;
	hit.triangleIndex = triangleIndex;

	vec3 radiance = vec3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	while (sampleIndex < SAMPLE_COUNT) {
		radiance += rgen(hit, seed);

		sampleIndex += 1;
	}

	vec3 light = radiance / SAMPLE_COUNT;

	imageStore(intermediateLightmapAtlas, coordinate, vec4(light, 1.0));
}