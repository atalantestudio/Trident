#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp image2D;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

struct Camera {
	mat4x4 inverseProjection;
	mat4x4 inverseView;
	vec3 position;
	uvec2 viewport;
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
	vec3 origin;
	vec3 direction;
};

struct Hit {
	float t;
	vec3 position;
	vec3 normal;
};

const float FLOAT32_MAX = 4294967295.0;
const uint RAY_BOUNCE_COUNT = 8;
const vec3 SKY_COLOR = vec3(1.0, 1.0, 1.0);

layout(set = 0, binding = 0) uniform readonly CameraBuffer {
	Camera camera;
};

layout(set = 0, binding = 1) buffer readonly TriangleBuffer {
	Triangle triangleBuffer[];
};

layout(set = 0, binding = 2) buffer readonly VertexBuffer {
	float vertexBuffer[];
};

layout(set = 0, binding = 3) buffer readonly NormalBuffer {
	float normalBuffer[];
};

layout(set = 0, binding = 4) buffer readonly TextureCoordinateBuffer {
	float textureCoordinateBuffer[];
};

layout(set = 0, binding = 5) buffer AccumulateBuffer {
	vec4 accumulateBuffer[];
};

layout(set = 0, binding = 6, rgba8) uniform writeonly image2D outputImage;

vec2 ndc(vec2 coordinate) {
	vec2 ndcCoordinate = coordinate / camera.viewport * 2.0 - 1.0;
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

vec3 randomUnitDirection(inout uint seed) {
	return normalize(vec3(randomFloat(seed), randomFloat(seed), randomFloat(seed)) * 2.0 - 1.0);
}

bool rayTriangleIntersection(Ray ray, Triangle triangle_, inout float t) {
	vec3 a = vec3(
		vertexBuffer[triangle_.vertices[0].index * 3 + 0],
		vertexBuffer[triangle_.vertices[0].index * 3 + 1],
		vertexBuffer[triangle_.vertices[0].index * 3 + 2]
	);
	vec3 b = vec3(
		vertexBuffer[triangle_.vertices[1].index * 3 + 0],
		vertexBuffer[triangle_.vertices[1].index * 3 + 1],
		vertexBuffer[triangle_.vertices[1].index * 3 + 2]
	);
	vec3 c = vec3(
		vertexBuffer[triangle_.vertices[2].index * 3 + 0],
		vertexBuffer[triangle_.vertices[2].index * 3 + 1],
		vertexBuffer[triangle_.vertices[2].index * 3 + 2]
	);
	vec3 normal = vec3(
		normalBuffer[triangle_.normalIndex * 3 + 0],
		normalBuffer[triangle_.normalIndex * 3 + 1],
		normalBuffer[triangle_.normalIndex * 3 + 2]
	);

	const float epsilon = 0.0001;

	if (dot(normal, ray.direction) > epsilon) {
		// Ignore triangles that are near-parallel to the ray,
		// and apply back-face culling if necessary.
		return false;
	}

	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 pvec = cross(ray.direction, ac);

	// d must be positive.
	float d = dot(ab, pvec);

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

	t = dot(ac, qvec) * invD;

	if (t < 0.0) {
		return false;
	}

	return true;
}

Hit rchit(Ray ray, Triangle triangle_, float t) {
	vec3 normal = vec3(
		normalBuffer[triangle_.normalIndex * 3 + 0],
		normalBuffer[triangle_.normalIndex * 3 + 1],
		normalBuffer[triangle_.normalIndex * 3 + 2]
	);

	Hit hit;
	hit.t = t;
	hit.position = ray.origin + ray.direction * t;
	hit.normal = normal;

	return hit;
}

Hit rmiss() {
	Hit hit;
	hit.t = -1.0;

	return hit;
}

Hit rint(Ray ray) {
	bool intersecting = false;
	Triangle intersectingTriangle;
	float closestT = FLOAT32_MAX;
	uint triangleIndex = 0;

	while (triangleIndex < camera.triangleCount) {
		Triangle triangle_ = triangleBuffer[triangleIndex];
		float t;

		if (rayTriangleIntersection(ray, triangle_, t)) {
			intersecting = true;

			if (t < closestT) {
				intersectingTriangle = triangle_;
				closestT = t;
			}
		}

		triangleIndex += 1;
	}

	if (intersecting) {
		return rchit(ray, intersectingTriangle, closestT);
	}

	return rmiss();
}

vec4 rgen(uvec2 coordinate, inout uint seed) {
	vec2 ndcTextureCoordinate = ndc(coordinate);
	vec4 position = vec4((camera.inverseProjection * vec4(ndcTextureCoordinate, 0.0, 1.0)).xyz, 0.0);

	vec3 light = vec3(0.0, 0.0, 0.0);
	vec3 throughput = vec3(1.0, 1.0, 1.0);

	Ray ray;
	ray.origin = camera.position;
	ray.direction = normalize((camera.inverseView * position).xyz);

	uint bounceIndex = 0;

	while (bounceIndex < RAY_BOUNCE_COUNT) {
		seed += bounceIndex;

		Hit hit = rint(ray);

		if (hit.t < 0.0) {
			light += SKY_COLOR * throughput;

			break;
		}

		const vec3 albedo = vec3(0.9, 0.9, 0.9);
		const float roughness = 1.0;

		throughput *= albedo;

		vec3 diffuse = normalize(hit.normal + randomUnitDirection(seed));
		vec3 specular = reflect(ray.direction, hit.normal);

		ray.origin = hit.position + hit.normal * 0.0001;
		ray.direction = mix(specular, diffuse, roughness);

		bounceIndex += 1;
	}

	return vec4(light, 1.0);
}

void main() {
	uvec2 coordinate = gl_GlobalInvocationID.xy;

	if (coordinate.x >= camera.viewport.x || coordinate.y >= camera.viewport.y) {
		return;
	}

	uint coordinateIndex = (coordinate.y * camera.viewport.x + coordinate.x);
	uint seed = coordinateIndex * camera.frameIndex;
	vec4 sample_ = rgen(coordinate, seed);

	if (camera.frameIndex == 0) {
		accumulateBuffer[coordinateIndex] = sample_;

		imageStore(outputImage, ivec2(coordinate), sample_);
	}
	else {
		accumulateBuffer[coordinateIndex] += sample_;

		imageStore(outputImage, ivec2(coordinate), accumulateBuffer[coordinateIndex] / camera.frameIndex);
	}
}