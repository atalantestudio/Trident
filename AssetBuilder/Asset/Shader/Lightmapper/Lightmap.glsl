// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#extension GL_EXT_ray_tracing : enable

struct Arguments {
	uvec2 atlasSize;
	uint emissiveTriangleCount;
};

struct Triangle {
	uint index;
	uint lightmapCoordinateIndexA;
	uint lightmapCoordinateIndexB;
	uint lightmapCoordinateIndexC;
	uint materialIndex;
};

struct Instance {
	mat4x3 transform;
	uint geometryTriangleOffset;
	uint deduplicatedTriangleOffset;
	uint lightmapCoordinateOffset;
};

struct Material {
	vec3 emissiveColor;
	float emissiveIntensity;
	vec3 albedo;
	uint _0;
};

struct Ray {
	vec3 origin;
	float tMin;
	vec3 direction;
	float tMax;
};

struct Hit {
	vec3 position;
	float t;
	vec3 triangleNormal;
	uint triangleIndex;
	vec3 vertexNormal;
	uint materialIndex;
};

const float PI = radians(180.0);
const float FLOAT_MAX = 4294967295.0;
const uint SAMPLE_COUNT = 1024;
const uint BOUNCE_COUNT = 4;
const float ADAPTIVE_BIAS = 0.0000002;

layout(set = 0, binding = 0) uniform accelerationStructureEXT accelerationStructure;

layout(set = 0, binding = 1) uniform readonly ArgumentBuffer {
	Arguments arguments;
};

layout(set = 0, binding = 2) buffer readonly LightmapCoordinateBuffer {
	vec2 lightmapCoordinateBuffer[];
};

layout(set = 0, binding = 3) buffer readonly IndexBuffer {
	uint indexBuffer[];
};

layout(set = 0, binding = 4) buffer readonly EmissiveTriangleIndexBuffer {
	uint emissiveTriangleIndexBuffer[];
};

layout(set = 0, binding = 5) buffer readonly TriangleBuffer {
	Triangle triangleBuffer[];
};

layout(set = 0, binding = 6) buffer readonly InstanceBuffer {
	Instance instanceBuffer[];
};

layout(set = 0, binding = 7) buffer readonly MaterialBuffer {
	Material materialBuffer[];
};

layout(set = 0, binding = 8) uniform samplerCube environmentMap;

layout(set = 0, binding = 9, rgba32f) uniform readonly image2D gBufferPositionImage;
layout(set = 0, binding = 10, rgba32f) uniform readonly image2D gBufferSmoothPositionImage;
layout(set = 0, binding = 11, rgba32f) uniform readonly image2D gBufferTriangleNormalImage;
layout(set = 0, binding = 12, rgba32f) uniform readonly image2D gBufferVertexNormalImage;

layout(set = 0, binding = 13, rgba8) uniform writeonly image2D intermediateLightmapAtlas;