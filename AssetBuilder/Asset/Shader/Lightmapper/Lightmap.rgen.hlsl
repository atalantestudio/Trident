// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct TraceRaysArguments {
	uint2 atlasSize;
	uint triangleCount;
	uint _0;
	float3 skyColor;
	uint _1;
	float3 sunDirection;
	uint _2;
	float3 sunColor;
	float sunIntensity;
};

struct Material {
	float3 emissiveColor;
	float emissiveIntensity;
	float3 albedo;
};

RaytracingAccelerationStructure tlas : register(t0);
StructuredBuffer<Arguments> argumentBuffer : register(t1);
StructuredBuffer<Material> materialBuffer : register(t2);
StructuredBuffer<uint> emissiveTriangleBuffer : register(t3);
Texture2D<float4> gBufferPositionImage : register(t4);
Texture2D<float4> gBufferSmoothPositionImage : register(t5);
Texture2D<float4> gBufferTriangleNormalImage : register(t6);
Texture2D<float4> gBufferVertexNormalImage : register(t7);
RWTexture2D<float4> intermediateLightmapAtlas : register(u0);

struct Payload {
	[[vk::location(0)]] float3 hitValue;
};

[shader("raygeneration")]
void main() {
	uint2 coordinate = DispatchRaysIndex();

	intermediateLightmapAtlas[coordinate] = float4(1.0, 0.2, 0.0, 1.0));
}