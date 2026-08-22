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

struct Face {
	vec3 emissiveColor;
	float emissiveIntensity;
	vec3 albedo;
};

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Hit {
	vec3 position;
	float distance;
	vec3 triangleNormal;
	uint faceIndex;
	vec3 vertexNormal;
	uint triangleIndex;
};

struct BarycentricCoordinate {
	float u;
	float v;
	float w;
};

const float PI = radians(180.0);
const float FLOAT32_MAX = 4294967295.0;
const uint SAMPLE_COUNT = 1024;
const uint BOUNCE_COUNT = 4;
const float ADAPTIVE_BIAS = 0.0000002;

// TODO(wip)
#if 1
	const uint emissiveTriangleCount = 2;
	const uint emissiveTriangleIndexBuffer[emissiveTriangleCount] = {2, 3};
#endif

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

layout(set = 0, binding = 3) buffer readonly FaceBuffer {
	Face faceBuffer[];
};

layout(set = 0, binding = 4, rgba32f) uniform readonly image2D gBufferPositionImage;

layout(set = 0, binding = 5, rgba32f) uniform readonly image2D gBufferSmoothPositionImage;

layout(set = 0, binding = 6, rgba32f) uniform readonly image2D gBufferTriangleNormalImage;

layout(set = 0, binding = 7, rgba32f) uniform readonly image2D gBufferVertexNormalImage;

layout(set = 0, binding = 8, rgba8) uniform writeonly image2D intermediateLightmapAtlas;

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

float lengthSquared(vec3 a) {
	return dot(a, a);
}

vec3 interpolate3x3(BarycentricCoordinate coordinate, vec3 a, vec3 b, vec3 c) {
	return a * coordinate.u + b * coordinate.v + c * coordinate.w;
}

float balanceHeuristic(float pdf1, float pdf2) {
	return pdf1 / (pdf1 + pdf2);
}


bool rayTriangleIntersectionFrontFacing(Ray ray, Triangle triangle_, inout float t, inout vec3 barycentricCoordinate) {
	const float E = 1e-6;
	const float F = 1e-6;

	vec3 a = triangle_.vertices[0].position;
	vec3 b = triangle_.vertices[1].position;
	vec3 c = triangle_.vertices[2].position;
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 p = cross(ray.direction, ac);
	float d = dot(ab, p);

	if (d <= E) {
		return false;
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

bool rayTriangleIntersectionBackFacing(Ray ray, Triangle triangle_, inout float t, inout vec3 barycentricCoordinate) {
	const float E = 1e-6;
	const float F = 1e-6;

	vec3 a = triangle_.vertices[0].position;
	vec3 b = triangle_.vertices[1].position;
	vec3 c = triangle_.vertices[2].position;
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 p = cross(ray.direction, ac);
	float d = dot(ab, p);

	if (d >= -E) {
		return false;
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

Hit rchit(Ray ray, uint triangleIndex, Triangle triangle_, float distance, vec3 barycentricCoordinate) {
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
	hit.faceIndex = uint(vertexNormalData.a);
	hit.vertexNormal = vertexNormalData.rgb;
	hit.triangleIndex = triangleIndex;

	return hit;
}

Hit rmiss() {
	Hit hit;
	hit.distance = -1.0;

	return hit;
}

Hit rintFrontFacing(Ray ray) {
	float closestDistance = FLOAT32_MAX;
	uint intersectingTriangleIndex = triangleCount;
	Triangle intersectingTriangle;
	vec3 intersectingBarycentricCoordinate;
	uint triangleIndex = 0;

	while (triangleIndex < triangleCount) {
		Triangle triangle_ = loadTriangle(triangleIndex);
		float distance;
		vec3 barycentricCoordinate;

		if (rayTriangleIntersectionFrontFacing(ray, triangle_, distance, barycentricCoordinate)) {
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
		return rchit(ray, intersectingTriangleIndex, intersectingTriangle, closestDistance, intersectingBarycentricCoordinate);
	}

	return rmiss();
}

Hit rintBackFacing(Ray ray) {
	float closestDistance = FLOAT32_MAX;
	uint intersectingTriangleIndex = triangleCount;
	Triangle intersectingTriangle;
	vec3 intersectingBarycentricCoordinate;
	uint triangleIndex = 0;

	while (triangleIndex < triangleCount) {
		Triangle triangle_ = loadTriangle(triangleIndex);
		float distance;
		vec3 barycentricCoordinate;

		if (rayTriangleIntersectionBackFacing(ray, triangle_, distance, barycentricCoordinate)) {
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
		return rchit(ray, intersectingTriangleIndex, intersectingTriangle, closestDistance, intersectingBarycentricCoordinate);
	}

	return rmiss();
}

/// Generate ray info for a random emissive triangle.
/// The sample point is returned in `p`.
void DirectLightSampling(inout uint emissiveTriangleIndex, inout vec3 p, inout float pdf, inout uint seed) {
	uint emissiveTriangleIndexIndex = uint(randomScalar(seed) * emissiveTriangleCount);

	emissiveTriangleIndex = emissiveTriangleIndexBuffer[emissiveTriangleIndexIndex];

	Triangle emissiveTriangle = loadTriangle(emissiveTriangleIndex);

	vec3 a = emissiveTriangle.vertices[0].position;
	vec3 b = emissiveTriangle.vertices[1].position;
	vec3 c = emissiveTriangle.vertices[2].position;

	float u = randomScalar(seed);
	float v = randomScalar(seed);

	if (u + v > 1.0) {
		u = 1.0 - u;
		v = 1.0 - v;
	}

	BarycentricCoordinate coordinate = {u, v, 1.0 - u - v};

	p = interpolate3x3(coordinate, a, b, c);

	float abc = length(cross(b - a, c - a)) * 0.5;

	pdf = 1.0 / emissiveTriangleCount / abc;
}

float PDFLight(uint triangleIndex) {
	Triangle triangle_ = loadTriangle(triangleIndex);

	vec3 a = triangle_.vertices[0].position;
	vec3 b = triangle_.vertices[1].position;
	vec3 c = triangle_.vertices[2].position;

	float abc = length(cross(b - a, c - a)) * 0.5;

	return 1.0 / emissiveTriangleCount / abc;
}

vec3 evaluatePhongMaterial(Face face) {
	return face.albedo / PI;
}

vec3 rgen(Hit primaryHit, inout uint seed) {
	const float pdfDiffuse = 1.0 / (4.0 * PI);

	vec3 incomingLight = vec3(0.0, 0.0, 0.0);
	vec3 throughput = vec3(1.0, 1.0, 1.0);
	uint bounceIndex = 0;

	while (bounceIndex < BOUNCE_COUNT) {
		Face material = faceBuffer[primaryHit.faceIndex];

		vec3 o = primaryHit.position + sign(primaryHit.vertexNormal) * abs(primaryHit.position * ADAPTIVE_BIAS);

		Ray secondaryRay;
		secondaryRay.origin = o;

		uint emissiveTriangleIndex;
		vec3 p;
		float pdfLight;

		DirectLightSampling(emissiveTriangleIndex, p, pdfLight, seed);

		vec3 op = p - o;

		secondaryRay.direction = normalize(op);

		vec3 directLight = vec3(0.0, 0.0, 0.0);

		if (dot(primaryHit.vertexNormal, secondaryRay.direction) > 0.001) {
			Hit secondaryHit = rintFrontFacing(secondaryRay);

			if (secondaryHit.triangleIndex == emissiveTriangleIndex) {
				Face material = faceBuffer[secondaryHit.faceIndex];

				float angleA = dot(secondaryRay.direction, primaryHit.vertexNormal);
				float angleB = max(0.001, abs(dot(secondaryRay.direction, secondaryHit.vertexNormal)));
				float G = 1.0 / lengthSquared(op) * angleA * angleB;
				float w = balanceHeuristic(pdfLight, pdfDiffuse * G);

				directLight = material.emissiveColor * material.emissiveIntensity * G / pdfLight * w;
			}
		}

		if (material.emissiveIntensity > 0.0) {
			if (bounceIndex == 0) {
				incomingLight += material.emissiveColor * material.emissiveIntensity * throughput;
			}

			break;
		}

		Ray primaryRay;
		primaryRay.origin = o;
		primaryRay.direction = normalize(primaryHit.vertexNormal + randomUnitDirection(seed));

		float cosTheta = max(0.001, dot(primaryHit.vertexNormal, primaryRay.direction));

		primaryHit = rintFrontFacing(primaryRay);

		if (primaryHit.distance < 0.0) {
			incomingLight += skyColor * throughput;

			break;
		}

		vec3 brdf = evaluatePhongMaterial(material);

		incomingLight += directLight * throughput * brdf;
		throughput *= material.albedo;

		bounceIndex += 1;
	}

	return incomingLight;
}

bool adjustSamplePosition(inout vec3 position, vec3 triangleNormal, float diagonalLuxelSize) {
	const float bias = 1e-4;

	vec3 a = normalize(cross(position, triangleNormal));
	vec3 b = normalize(cross(a, triangleNormal));
	float rayLength = diagonalLuxelSize * 0.5;

	Ray ray;
	ray.origin = position + sign(triangleNormal) * abs(position * bias);
	ray.direction = a;

	Hit hit = rintBackFacing(ray);

	uint obstacleCount = uint(hit.distance > 0.0);

	if (hit.distance > 0.0) {
		if (hit.distance <= rayLength) {
			position = hit.position + sign(hit.triangleNormal) * abs(hit.position * bias);

			return false;
		}
	}

	ray.direction = b;

	hit = rintBackFacing(ray);

	obstacleCount += uint(hit.distance > 0.0);

	if (hit.distance > 0.0) {
		if (hit.distance <= rayLength) {
			position = hit.position + sign(hit.triangleNormal) * abs(hit.position * bias);

			return false;
		}
	}

	ray.direction = -a;
	hit = rintBackFacing(ray);

	obstacleCount += uint(hit.distance > 0.0);

	if (hit.distance > 0.0) {
		if (hit.distance <= rayLength) {
			position = hit.position + sign(hit.triangleNormal) * abs(hit.position * bias);

			return false;
		}
	}

	ray.direction = -b;
	hit = rintBackFacing(ray);

	obstacleCount += uint(hit.distance > 0.0);

	if (hit.distance > 0.0) {
		if (hit.distance <= rayLength) {
			position = hit.position + sign(hit.triangleNormal) * abs(hit.position * bias);

			return false;
		}
	}

	return obstacleCount == 4;
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

	if (!adjustSamplePosition(position, triangleNormal, diagonalLuxelSize)) {
		return;
	}

	Hit hit;
	hit.position = position;
	hit.triangleNormal = triangleNormal;
	hit.faceIndex = uint(vertexNormalData.w);
	hit.vertexNormal = vertexNormal;

	vec3 radiance = vec3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	while (sampleIndex < SAMPLE_COUNT) {
		radiance += rgen(hit, seed);

		sampleIndex += 1;
	}

	vec3 light = radiance / SAMPLE_COUNT;
	light = pow(light, vec3(1.0 / 2.2));

	imageStore(intermediateLightmapAtlas, coordinate, vec4(light, 1.0));
}