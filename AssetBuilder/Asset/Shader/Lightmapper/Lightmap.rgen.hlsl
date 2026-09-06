// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Arguments {
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
StructuredBuffer<uint> emissiveTriangleIndexBuffer : register(t3);

Texture2D<float4> gBufferPositionImage : register(t4);
Texture2D<float4> gBufferSmoothPositionImage : register(t5);
Texture2D<float4> gBufferTriangleNormalImage : register(t6);
Texture2D<float4> gBufferVertexNormalImage : register(t7);

RWTexture2D<float4> intermediateLightmapAtlas : register(u0);

struct Payload {
	float3 hitValue;
};

[shader("raygeneration")]
void main() {
	uint2 coordinate = DispatchRaysIndex().xy;

	float4 gBufferPosition = gBufferPositionImage[coordinate];
	float4 gBufferSmoothPosition = gBufferSmoothPositionImage[coordinate];
	float4 gBufferTriangleNormal = gBufferTriangleNormalImage[coordinate];
	float4 gBufferVertexNormal = gBufferVertexNormalImage[coordinate];

	intermediateLightmapAtlas[coordinate] = float4(abs(gBufferTriangleNormal.xyz), 1.0);
}