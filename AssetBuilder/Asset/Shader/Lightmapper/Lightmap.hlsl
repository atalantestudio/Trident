// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Arguments {
	uint2 atlasSize;

	// TODO(asset-builder/lightmapper): Remove.
	uint triangleCount;

	uint emissiveTriangleCount;
	float3 skyColor;

	// TODO(asset-builder/lightmapper): Remove.
	uint _0;
	float3 sunDirection;
	uint _1;
	float3 sunColor;
	float sunIntensity;
};

struct Vertex {
	float3 position;
	uint materialIndex;
	float3 normal;
};

struct Instance {
	float4x3 transform;
	uint geometryTriangleOffset;
	uint geometryVertexOffset;
	uint lightmapCoordinateOffset;
};

struct Material {
	float3 emissiveColor;
	float emissiveIntensity;
	float3 albedo;
};

struct Hit {
	float3 position;
	float t;
	float3 triangleNormal;
	uint materialIndex;
	float3 vertexNormal;
};

static const float PI = radians(180.0);
static const float FLOAT_MAX = 4294967295.0;
static const uint SAMPLE_COUNT = 1024;
static const uint BOUNCE_COUNT = 4;
static const float ADAPTIVE_BIAS = 0.0000002;

RaytracingAccelerationStructure accelerationStructure : register(t0);
StructuredBuffer<Arguments> argumentBuffer : register(t1);
StructuredBuffer<Vertex> vertexBuffer : register(t2);
StructuredBuffer<float2> lightmapCoordinateBuffer : register(t3);
StructuredBuffer<uint> indexBuffer : register(t4);
StructuredBuffer<Instance> instanceBuffer : register(t5);
StructuredBuffer<Material> materialBuffer : register(t6);
StructuredBuffer<uint> emissiveTriangleIndexBuffer : register(t7);

TextureCube<float4> environmentMapTexture : register(t8);
SamplerState environmentMapSamplerState : register(s0);

Texture2D<float4> gBufferPositionImage : register(t9);
Texture2D<float4> gBufferSmoothPositionImage : register(t10);
Texture2D<float4> gBufferTriangleNormalImage : register(t11);
Texture2D<float4> gBufferVertexNormalImage : register(t12);

RWTexture2D<float4> intermediateLightmapAtlas : register(u0);

void pcgHash(inout uint seed) {
	uint state = seed * 747796405 + 2891336453;
	uint word = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;

	seed = (word >> 22) ^ word;
}

float randomScalar(inout uint seed) {
	pcgHash(seed);

	return seed / FLOAT_MAX;
}

float3 randomUnitDirection(inout uint seed) {
	return normalize(float3(randomScalar(seed), randomScalar(seed), randomScalar(seed)) * 2.0 - 1.0);
}