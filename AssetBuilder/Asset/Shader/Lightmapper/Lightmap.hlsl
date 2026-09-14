// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Arguments {
	uint2 atlasSize;
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
	float4x3 transform;
	uint geometryTriangleOffset;
	uint deduplicatedTriangleOffset;
	uint lightmapCoordinateOffset;
};

struct Material {
	float3 emissiveColor;
	float emissiveIntensity;
	float3 albedo;
	uint _0;
};

struct Hit {
	float3 position;
	float t;
	float3 triangleNormal;
	uint triangleIndex;
	float3 vertexNormal;
	uint materialIndex;
};

static const float PI = radians(180.0);
static const float FLOAT_MAX = 4294967295.0;
static const uint SAMPLE_COUNT = 1024;
static const uint BOUNCE_COUNT = 4;
static const float ADAPTIVE_BIAS = 0.0000002;

RaytracingAccelerationStructure accelerationStructure : register(t0);

cbuffer Arguments : register(b0) {
	Arguments arguments;
}

StructuredBuffer<float2> lightmapCoordinateBuffer : register(t1);
StructuredBuffer<uint> indexBuffer : register(t2);
StructuredBuffer<uint> emissiveTriangleIndexBuffer : register(t3);
StructuredBuffer<Triangle> triangleBuffer : register(t4);
StructuredBuffer<Instance> instanceBuffer : register(t5);
StructuredBuffer<Material> materialBuffer : register(t6);

TextureCube<float4> environmentMapTexture : register(t7);
SamplerState environmentMapSamplerState : register(s0);

Texture2D<float4> gBufferPositionImage : register(t8);
Texture2D<float4> gBufferSmoothPositionImage : register(t9);
Texture2D<float4> gBufferTriangleNormalImage : register(t10);
Texture2D<float4> gBufferVertexNormalImage : register(t11);

RWTexture2D<float4> intermediateLightmapAtlas : register(u0);