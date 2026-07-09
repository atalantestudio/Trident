#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 16, local_size_y = 1, local_size_z = 1) in;

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
	vec3 origin;
	vec3 direction;
};

struct Hit {
	float distance;
	vec3 position;
	vec3 normal;
};

const float FLOAT32_MAX = 4294967295.0;
const uint SAMPLE_COUNT = 1024;
const uint BOUNCE_COUNT = 32;
const uvec2 ATLAS_SIZE = uvec2(2048, 2048);

layout(set = 0, binding = 0) buffer readonly TraceRaysArgumentBuffer {
	uint lightmapPixelCount;
	uint triangleCount;
};

layout(set = 0, binding = 1) buffer readonly LightmapPixelBuffer {
	LightmapPixel lightmapPixels[];
};

layout(set = 0, binding = 2) buffer readonly TriangleBuffer {
	Triangle triangles[];
};

layout(set = 0, binding = 3) buffer readonly VertexComponents {
	float vertexComponents[];
};

layout(set = 0, binding = 4) buffer readonly NormalComponentBuffer {
	float normalComponents[];
};

layout(set = 0, binding = 5) buffer readonly LightmapCoordinateBuffer {
	vec2 lightmapCoordinates[];
};

layout(set = 0, binding = 6, rgba8) uniform writeonly image2D lightmapAtlasImage;

float parallelogramArea(vec2 a, vec2 b) {
	return a.x * b.y - a.y * b.x;
}

vec3 calculateBarycentricCoordinate(vec2 a, vec2 b, vec2 c, vec2 p) {
	vec2 ab = b - a;
	vec2 ac = c - a;
	vec2 ap = p - a;
	float inverseArea = 1.0 / parallelogramArea(ab, ac);
	float v = parallelogramArea(ap, ac) * inverseArea;
	float w = parallelogramArea(ab, ap) * inverseArea;
	float u = 1.0 - v - w;

	return vec3(u, v, w);
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

vec3 randomUnitDirection(inout uint seed) {
	return normalize(vec3(randomFloat(seed), randomFloat(seed), randomFloat(seed)) * 2.0 - 1.0);
}

bool rayTriangleIntersection(Ray ray, Triangle triangle_, inout float distance) {
	uint normalOffset = triangle_.normalIndex * 3;
	vec3 normal = vec3(normalComponents[normalOffset + 0], normalComponents[normalOffset + 1], normalComponents[normalOffset + 2]);

	if (dot(ray.direction, normal) > 0.0) {
		return false;
	}

	uint vertexAOffset = triangle_.vertices[0].index * 3;
	uint vertexBOffset = triangle_.vertices[1].index * 3;
	uint vertexCOffset = triangle_.vertices[2].index * 3;
	vec3 a = vec3(vertexComponents[vertexAOffset + 0], vertexComponents[vertexAOffset + 1], vertexComponents[vertexAOffset + 2]);
	vec3 b = vec3(vertexComponents[vertexBOffset + 0], vertexComponents[vertexBOffset + 1], vertexComponents[vertexBOffset + 2]);
	vec3 c = vec3(vertexComponents[vertexCOffset + 0], vertexComponents[vertexCOffset + 1], vertexComponents[vertexCOffset + 2]);
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 pvec = cross(ray.direction, ac);
	float d = dot(ab, pvec);

	if (abs(d) < 0.0001) {
		return false;
	}

	float invD = 1.0 / d;
	vec3 tvec = ray.origin - a;
	float u = dot(tvec, pvec) * invD;

	if (u < 0.0 || u > 1.0) {
		return false;
	}

	vec3 qvec = cross(tvec, ab);
	float v = dot(ray.direction, qvec) * invD;

	if (v < 0.0 || u + v > 1.0) {
		return false;
	}

	distance = dot(ac, qvec) * invD;

	if (distance < 0.0) {
		return false;
	}

	return true;
}

Hit rchit(Ray ray, uint triangleIndex, float distance) {
	Triangle triangle_ = triangles[triangleIndex];
	uint normalOffset = triangle_.normalIndex * 3;
	vec3 normal = vec3(normalComponents[normalOffset + 0], normalComponents[normalOffset + 1], normalComponents[normalOffset + 2]);

	Hit hit;
	hit.distance = distance;
	hit.position = ray.origin + ray.direction * distance;
	hit.normal = normal;

	return hit;
}

Hit rmiss() {
	Hit hit;
	hit.distance = -1.0;

	return hit;
}

Hit rint(Ray ray) {
	float closestDistance = FLOAT32_MAX;
	uint intersectingTriangleIndex = triangleCount;
	uint triangleIndex = 0;

	while (triangleIndex < triangleCount) {
		Triangle triangle_ = triangles[triangleIndex];
		float distance;

		if (rayTriangleIntersection(ray, triangle_, distance)) {
			if (distance < closestDistance) {
				closestDistance = distance;
				intersectingTriangleIndex = triangleIndex;
			}
		}

		triangleIndex += 1;
	}

	if (intersectingTriangleIndex < triangleCount) {
		return rchit(ray, intersectingTriangleIndex, closestDistance);
	}

	return rmiss();
}

vec3 rgen(Hit hit, inout uint seed) {
	Ray ray;
	vec3 light = vec3(0.0, 0.0, 0.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		ray.direction = normalize(hit.normal + randomUnitDirection(seed));

		// Smaller numbers like 0.0001 cause visual artifacts.
		ray.origin = hit.position + ray.direction * 0.001;

		hit = rint(ray);

		if (hit.distance < 0.0) {
			light += vec3(1.0, 1.0, 1.0);

			break;
		}

		bounceIndex += 1;
	}

	return light;
}

void main() {
	uint pixelIndex = gl_GlobalInvocationID.x;

	if (pixelIndex >= lightmapPixelCount) {
		return;
	}

	LightmapPixel pixel = lightmapPixels[pixelIndex];

	uvec2 coordinate = uvec2(pixel.coordinate >> 16, pixel.coordinate & 0xffff);
	uint seed = coordinate.y * ATLAS_SIZE.x + coordinate.x;
	Triangle triangle_ = triangles[pixel.triangleIndex];
	uint normalOffset = triangle_.normalIndex * 3;
	vec3 normal = vec3(normalComponents[normalOffset + 0], normalComponents[normalOffset + 1], normalComponents[normalOffset + 2]);
	uint vertexAOffset = triangle_.vertices[0].index * 3;
	uint vertexBOffset = triangle_.vertices[1].index * 3;
	uint vertexCOffset = triangle_.vertices[2].index * 3;
	vec3 vertexA = vec3(vertexComponents[vertexAOffset + 0], vertexComponents[vertexAOffset + 1], vertexComponents[vertexAOffset + 2]);
	vec3 vertexB = vec3(vertexComponents[vertexBOffset + 0], vertexComponents[vertexBOffset + 1], vertexComponents[vertexBOffset + 2]);
	vec3 vertexC = vec3(vertexComponents[vertexCOffset + 0], vertexComponents[vertexCOffset + 1], vertexComponents[vertexCOffset + 2]);
	vec2 lightmapCoordinateA = lightmapCoordinates[triangle_.vertices[0].lightmapCoordinateIndex];
	vec2 lightmapCoordinateB = lightmapCoordinates[triangle_.vertices[1].lightmapCoordinateIndex];
	vec2 lightmapCoordinateC = lightmapCoordinates[triangle_.vertices[2].lightmapCoordinateIndex];
	vec2 pixelCoordinate = (coordinate + vec2(0.5, 0.5)) / ATLAS_SIZE;
	vec3 pixelBarycentricCoordinate = calculateBarycentricCoordinate(lightmapCoordinateA, lightmapCoordinateB, lightmapCoordinateC, pixelCoordinate);
	vec3 light = vec3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	Hit hit;
	hit.position = vertexA * pixelBarycentricCoordinate.x + vertexB * pixelBarycentricCoordinate.y + vertexC * pixelBarycentricCoordinate.z;
	hit.normal = normal;

	while (sampleIndex < SAMPLE_COUNT) {
		light += rgen(hit, seed);

		sampleIndex += 1;
	}

	vec3 averageLight = light / SAMPLE_COUNT;

	imageStore(lightmapAtlasImage, ivec2(coordinate), vec4(averageLight, 1.0));
}