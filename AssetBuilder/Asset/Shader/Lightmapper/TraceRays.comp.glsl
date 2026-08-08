#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

struct Vertex {
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 lightmapCoordinate;
};

struct Triangle {
	Vertex vertices[3];
};

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Hit {
	vec3 position;
	float distance;
	vec3 triangleNormal;
	vec3 vertexNormal;
};

const float FLOAT32_MAX = 4294967295.0;
const uint SAMPLE_COUNT = 1024;
const uint BOUNCE_COUNT = 4;
const float ADAPTIVE_BIAS = 0.0000002;
const bool FRONT_FACE = true;
const bool BACK_FACE = false;

layout(set = 0, binding = 0) buffer readonly TraceRaysArgumentBuffer {
	uvec2 atlasSize;
	uint triangleCount;
	uint _0;
	vec3 skyColor;
	uint _1;
	vec3 sunDirection;
	uint _2;
	vec3 sunColor;
	float sunIntensity;
};

layout(set = 0, binding = 1) buffer readonly VertexBuffer {
	Vertex vertexBuffer[];
};

layout(set = 0, binding = 2) buffer readonly IndexBuffer {
	uint indexBuffer[];
};

layout(set = 0, binding = 3, rgba32f) uniform readonly image2D gBufferPositionImage;

layout(set = 0, binding = 4, rgba32f) uniform readonly image2D gBufferTriangleNormalImage;

layout(set = 0, binding = 5, rgba32f) uniform readonly image2D gBufferVertexNormalImage;

layout(set = 0, binding = 6, rgba8) uniform writeonly image2D intermediateLightmapAtlas;

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

vec3 randomUnitDirection(inout uint seed) {
	return normalize(vec3(randomScalar(seed), randomScalar(seed), randomScalar(seed)) * 2.0 - 1.0);
}

bool rayTriangleIntersection(Ray ray, Triangle triangle_, bool face, inout float t, inout vec3 barycentricCoordinate) {
	const float E = 1e-6;
	const float F = 1e-8;

	vec3 a = triangle_.vertices[0].position;
	vec3 b = triangle_.vertices[1].position;
	vec3 c = triangle_.vertices[2].position;
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 p = cross(ray.direction, ac);
	float d = dot(ab, p);

	if (face == FRONT_FACE) {
		if (d <= E) {
			return false;
		}
	}
	else {
		if (d >= -E) {
			return false;
		}
	}

	float inverseD = 1.0 / d;
	vec3 s = ray.origin - a;
	float u = dot(s, p) * inverseD;

	if (u < -F || u > 1.0 + F) {
		return false;
	}

	vec3 q = cross(s, ab);
	float v = dot(ray.direction, q) * inverseD;

	if (v < -F || u + v > 1.0 + F) {
		return false;
	}

	t = dot(ac, q) * inverseD;

	if (t > E) {
		float w = 1.0 - v - u;

		barycentricCoordinate = vec3(u, w, v);

		// TODO(asset-builder/lightmapper): This order also seems to work?
		// barycentricCoordinate = vec3(v, w, u);

		return true;
	}

	return false;
}

Hit rchit(Ray ray, Triangle triangle_, float distance, vec3 barycentricCoordinate) {
	ivec2 lightmapCoordinate = ivec2((
		triangle_.vertices[0].lightmapCoordinate * barycentricCoordinate.x +
		triangle_.vertices[1].lightmapCoordinate * barycentricCoordinate.y +
		triangle_.vertices[2].lightmapCoordinate * barycentricCoordinate.z
	) * atlasSize);

	vec4 triangleNormalData = imageLoad(gBufferTriangleNormalImage, lightmapCoordinate);
	vec4 vertexNormalData = imageLoad(gBufferVertexNormalImage, lightmapCoordinate);

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
	uint intersectingTriangleIndex = triangleCount;
	Triangle intersectingTriangle;
	vec3 intersectingBarycentricCoordinate;
	uint triangleIndex = 0;

	while (triangleIndex < triangleCount) {
		Triangle triangle_ = loadTriangle(triangleIndex);
		float distance;
		vec3 barycentricCoordinate;

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

	if (intersectingTriangleIndex < triangleCount) {
		return rchit(ray, intersectingTriangle, closestDistance, intersectingBarycentricCoordinate);
	}

	return rmiss();
}

vec3 rgen(Hit primaryHit, inout uint seed) {
	// TODO(asset-builder/lightmapper): Use the albedo of the triangle being hit.
	const vec3 albedo = skyColor;

	vec3 radiance = vec3(0.0, 0.0, 0.0);
	vec3 throughput = vec3(1.0, 1.0, 1.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		Ray primaryRay;
		primaryRay.origin = primaryHit.position + sign(primaryHit.vertexNormal) * abs(primaryHit.position * ADAPTIVE_BIAS);
		primaryRay.direction = normalize(primaryHit.vertexNormal + randomUnitDirection(seed));

		primaryHit = rint(primaryRay, FRONT_FACE);

		Ray secondaryRay;
		secondaryRay.origin = primaryRay.origin;
		secondaryRay.direction = normalize(sunDirection);

		Hit secondaryHit = rint(secondaryRay, FRONT_FACE);

		radiance += throughput * float(secondaryHit.distance < 0.0) * sunColor * sunIntensity;
		throughput *= albedo;

		if (primaryHit.distance < 0.0) {
			radiance += throughput * skyColor;

			break;
		}

		bounceIndex += 1;
	}

	return radiance;
}

vec3 adjustSamplePosition(vec3 position, vec3 triangleNormal, float diagonalLuxelSize) {
	const float bias = 1e-4;

	vec3 a = normalize(cross(position, triangleNormal));
	vec3 b = normalize(cross(a, triangleNormal));
	float rayLength = diagonalLuxelSize * 0.5;

	Ray ray;
	ray.origin = position + sign(triangleNormal) * abs(position * bias);
	ray.direction = a;

	Hit hit = rint(ray, BACK_FACE);

	if (hit.distance > 0.0 && hit.distance <= rayLength) {
		return hit.position + sign(hit.triangleNormal) * abs(hit.position * bias);
	}

	ray.direction = b;

	hit = rint(ray, BACK_FACE);

	if (hit.distance > 0.0 && hit.distance <= rayLength) {
		return hit.position + sign(hit.triangleNormal) * abs(hit.position * bias);
	}

	ray.direction = -a;
	hit = rint(ray, BACK_FACE);

	if (hit.distance > 0.0 && hit.distance <= rayLength) {
		return hit.position + sign(hit.triangleNormal) * abs(hit.position * bias);
	}

	ray.direction = -b;
	hit = rint(ray, BACK_FACE);

	if (hit.distance > 0.0 && hit.distance <= rayLength) {
		return hit.position + sign(hit.triangleNormal) * abs(hit.position * bias);
	}

	return position;
}

void main() {
	ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);

	if (coordinate.x >= atlasSize.x || coordinate.y >= atlasSize.y) {
		return;
	}

	uint seed = coordinate.y * atlasSize.x + coordinate.x;
	vec4 positionData = imageLoad(gBufferPositionImage, coordinate);
	vec4 triangleNormalData = imageLoad(gBufferTriangleNormalImage, coordinate);
	vec4 vertexNormalData = imageLoad(gBufferVertexNormalImage, coordinate);

	vec3 position = positionData.rgb;
	uint triangleIndex = uint(positionData.a);
	vec3 triangleNormal = triangleNormalData.rgb;
	float diagonalLuxelSize = triangleNormalData.a;
	vec3 vertexNormal = vertexNormalData.rgb;

	if (triangleIndex == 0) {
		return;
	}

	Hit hit;
	hit.position = adjustSamplePosition(position, triangleNormal, diagonalLuxelSize);
	hit.triangleNormal = triangleNormal;
	hit.vertexNormal = vertexNormal;

	vec3 radiance = vec3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	while (sampleIndex < SAMPLE_COUNT) {
		radiance += rgen(hit, seed);

		sampleIndex += 1;
	}

	vec3 light = radiance / SAMPLE_COUNT;

	imageStore(intermediateLightmapAtlas, coordinate, vec4(light, 1.0));
}