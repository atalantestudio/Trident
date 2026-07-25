#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 16, local_size_y = 1, local_size_z = 1) in;

struct Luxel {
	uint triangleIndex;
	uint coordinate;
};

struct Vertex {
	vec2 lightmapCoordinate;
	uint vertexIndex;
	uint normalIndex;
	uint textureCoordinateIndex;
	uint _;
};

struct Triangle {
	Vertex vertices[3];
	vec3 normal;
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
const uint BOUNCE_COUNT = 1;
const vec3 SUN_DIRECTION = vec3(-1.0, -1.0, -1.0);

layout(set = 0, binding = 0) buffer readonly TraceRaysArgumentBuffer {
	uvec2 atlasSize;
	uint luxelCount;
	uint triangleCount;
};

layout(set = 0, binding = 1) buffer readonly LuxelBuffer {
	Luxel luxels[];
};

layout(set = 0, binding = 2) buffer readonly IndexBuffer {
	Vertex indices[];
};

layout(set = 0, binding = 3) buffer readonly VertexComponentBuffer {
	float vertexComponents[];
};

layout(set = 0, binding = 4) buffer readonly NormalComponentBuffer {
	float normalComponents[];
};

layout(set = 0, binding = 5, rgba8) uniform writeonly image2D intermediateLightmapAtlas;

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

Triangle loadTriangle(uint triangleIndex) {
	uint indexOffset = triangleIndex * 3;

	Triangle triangle_;
	triangle_.vertices[0] = indices[indexOffset + 0];
	triangle_.vertices[1] = indices[indexOffset + 1];
	triangle_.vertices[2] = indices[indexOffset + 2];

	uint vertexAOffset = triangle_.vertices[0].vertexIndex * 3;
	uint vertexBOffset = triangle_.vertices[1].vertexIndex * 3;
	uint vertexCOffset = triangle_.vertices[2].vertexIndex * 3;
	vec3 a = vec3(vertexComponents[vertexAOffset + 0], vertexComponents[vertexAOffset + 1], vertexComponents[vertexAOffset + 2]);
	vec3 b = vec3(vertexComponents[vertexBOffset + 0], vertexComponents[vertexBOffset + 1], vertexComponents[vertexBOffset + 2]);
	vec3 c = vec3(vertexComponents[vertexCOffset + 0], vertexComponents[vertexCOffset + 1], vertexComponents[vertexCOffset + 2]);

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
	vec3 a = vec3(vertexComponents[vertexAOffset + 0], vertexComponents[vertexAOffset + 1], vertexComponents[vertexAOffset + 2]);
	vec3 b = vec3(vertexComponents[vertexBOffset + 0], vertexComponents[vertexBOffset + 1], vertexComponents[vertexBOffset + 2]);
	vec3 c = vec3(vertexComponents[vertexCOffset + 0], vertexComponents[vertexCOffset + 1], vertexComponents[vertexCOffset + 2]);
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 pvec = cross(ray.direction, ac);
	float d = dot(ab, pvec);

	if (abs(d) < 1e-4) {
		return false;
	}

	float invD = 1.0 / d;
	vec3 tvec = ray.origin - a;
	float u = dot(tvec, pvec) * invD;

	if (u < -1e-4 || u > 1.0 + 1e-4) {
		return false;
	}

	vec3 qvec = cross(tvec, ab);
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
	uint intersectingTriangleIndex = triangleCount;
	Triangle intersectingTriangle;
	uint triangleIndex = 0;

	while (triangleIndex < triangleCount) {
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

	if (intersectingTriangleIndex < triangleCount) {
		return rchit(ray, intersectingTriangle, closestDistance);
	}

	return rmiss();
}

vec3 rgen(Hit hit, inout uint seed) {
	Ray ray;

	Ray shadowRay;
	shadowRay.direction = normalize(-SUN_DIRECTION);

	Hit shadowHit;

	vec3 radiance = vec3(0.0, 0.0, 0.0);
	vec3 throughput = vec3(1.0, 1.0, 1.0);
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

		radiance += throughput * float(shadowHit.distance < 0.0);
		throughput *= 0.9;

		bounceIndex += 1;
	}

	return radiance;
}

void main() {
	uint luxelIndex = gl_GlobalInvocationID.x;

	if (luxelIndex >= luxelCount) {
		return;
	}

	Luxel luxel = luxels[luxelIndex];

	uvec2 coordinate = uvec2(luxel.coordinate >> 16, luxel.coordinate & 0xffff);
	uint seed = coordinate.y * atlasSize.x + coordinate.x;
	Triangle triangle_ = loadTriangle(luxel.triangleIndex);
	uint vertexAOffset = triangle_.vertices[0].vertexIndex * 3;
	uint vertexBOffset = triangle_.vertices[1].vertexIndex * 3;
	uint vertexCOffset = triangle_.vertices[2].vertexIndex * 3;
	vec3 vertexA = vec3(vertexComponents[vertexAOffset + 0], vertexComponents[vertexAOffset + 1], vertexComponents[vertexAOffset + 2]);
	vec3 vertexB = vec3(vertexComponents[vertexBOffset + 0], vertexComponents[vertexBOffset + 1], vertexComponents[vertexBOffset + 2]);
	vec3 vertexC = vec3(vertexComponents[vertexCOffset + 0], vertexComponents[vertexCOffset + 1], vertexComponents[vertexCOffset + 2]);
	vec2 luxelCoordinate = (coordinate + vec2(0.5, 0.5)) / atlasSize;
	vec3 luxelBarycentricCoordinate = calculateBarycentricCoordinate(triangle_.vertices[0].lightmapCoordinate, triangle_.vertices[1].lightmapCoordinate, triangle_.vertices[2].lightmapCoordinate, luxelCoordinate);
	vec3 radiance = vec3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	Hit hit;
	hit.position = vertexA * luxelBarycentricCoordinate.x + vertexB * luxelBarycentricCoordinate.y + vertexC * luxelBarycentricCoordinate.z;
	hit.normal = triangle_.normal;

	while (sampleIndex < SAMPLE_COUNT) {
		radiance += rgen(hit, seed);

		sampleIndex += 1;
	}

	vec3 light = radiance / SAMPLE_COUNT;

	imageStore(intermediateLightmapAtlas, ivec2(coordinate), vec4(light, 1.0));
}