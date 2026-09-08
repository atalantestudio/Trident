// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#extension GL_EXT_ray_tracing : enable

struct Arguments {
	uvec2 atlasSize;
	uint triangleCount;
	uint emissiveTriangleCount;
	vec3 skyColor;
	uint _0;
	vec3 sunDirection;
	uint _1;
	vec3 sunColor;
	float sunIntensity;
};

struct Vertex {
	vec3 position;
	uint _0;
	vec3 normal;
	uint _1;
	vec4 tangent;
	vec2 lightmapCoordinate;
	uvec2 _2;
};

struct Triangle {
	Vertex vertices[3];
};

struct Instance {
	uint triangleOffset;
	uint vertexOffset;
};

struct Material {
	vec3 emissiveColor;
	float emissiveIntensity;
	vec3 albedo;
};

struct Ray {
	vec3 origin;
	float tMin;
	vec3 direction;
	float tMax;
};

struct Hit {
	vec3 position;
	float distance;
	vec3 triangleNormal;
	uint materialIndex;
	vec3 vertexNormal;
};

const float PI = radians(180.0);
const float FLOAT_MAX = 4294967295.0;
const uint SAMPLE_COUNT = 1024;
const uint BOUNCE_COUNT = 4;
const float ADAPTIVE_BIAS = 0.0000002;

layout(set = 0, binding = 0) uniform accelerationStructureEXT tlas;

layout(set = 0, binding = 1) buffer readonly ArgumentBuffer {
	Arguments arguments;
};

layout(set = 0, binding = 2) buffer readonly VertexBuffer {
	Vertex vertexBuffer[];
};

layout(set = 0, binding = 3) buffer readonly IndexBuffer {
	uint indexBuffer[];
};

layout(set = 0, binding = 4) buffer readonly InstanceBuffer {
	Instance instanceBuffer[];
};

layout(set = 0, binding = 5) buffer readonly MaterialBuffer {
	Material materialBuffer[];
};

layout(set = 0, binding = 6) buffer readonly EmissiveTriangleIndexBuffer {
	uint emissiveTriangleIndexBuffer[];
};

layout(set = 0, binding = 7) uniform samplerCube environmentMap;

layout(set = 0, binding = 8, rgba32f) uniform readonly image2D gBufferPositionImage;
layout(set = 0, binding = 9, rgba32f) uniform readonly image2D gBufferSmoothPositionImage;
layout(set = 0, binding = 10, rgba32f) uniform readonly image2D gBufferTriangleNormalImage;
layout(set = 0, binding = 11, rgba32f) uniform readonly image2D gBufferVertexNormalImage;

layout(set = 0, binding = 12, rgba8) uniform writeonly image2D intermediateLightmapAtlas;

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

	return seed / FLOAT_MAX;
}

vec3 randomUnitDirection(inout uint seed) {
	return normalize(vec3(randomScalar(seed), randomScalar(seed), randomScalar(seed)) * 2.0 - 1.0);
}