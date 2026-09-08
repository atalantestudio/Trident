// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Arguments {
	uint2 atlasSize;
	uint emissiveTriangleCount;
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

cbuffer Arguments : register(b0) {
	Arguments arguments;
}

StructuredBuffer<Vertex> vertexBuffer : register(t1);
StructuredBuffer<float2> lightmapCoordinateBuffer : register(t2);
StructuredBuffer<uint> indexBuffer : register(t3);
StructuredBuffer<Instance> instanceBuffer : register(t4);
StructuredBuffer<Material> materialBuffer : register(t5);
StructuredBuffer<uint> emissiveTriangleIndexBuffer : register(t6);

TextureCube<float4> environmentMapTexture : register(t7);
SamplerState environmentMapSamplerState : register(s0);

Texture2D<float4> gBufferPositionImage : register(t8);
Texture2D<float4> gBufferSmoothPositionImage : register(t9);
Texture2D<float4> gBufferTriangleNormalImage : register(t10);
Texture2D<float4> gBufferVertexNormalImage : register(t11);

RWTexture2D<float4> intermediateLightmapAtlas : register(u0);