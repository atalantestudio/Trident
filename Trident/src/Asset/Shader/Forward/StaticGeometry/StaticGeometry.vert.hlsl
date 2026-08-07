// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	float3 position : VERTEX_INPUT_ATTRIBUTE0;
	float3 normal : VERTEX_INPUT_ATTRIBUTE1;
	float3 tangent : VERTEX_INPUT_ATTRIBUTE2;
	float2 textureCoordinate : VERTEX_INPUT_ATTRIBUTE3;
	float2 lightmapCoordinate : VERTEX_INPUT_ATTRIBUTE4;
	float4x4 worldTransform : VERTEX_INPUT_ATTRIBUTE5;
	float3x3 normalTransform : VERTEX_INPUT_ATTRIBUTE9;
};

struct Interstage {
	float4 position : SV_Position;
	float3 normal : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
	float2 lightmapCoordinate : VERTEX_OUTPUT_ATTRIBUTE3;
};

struct Camera {
	float4x4 projectionView;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

Interstage main(VertexInput input) {
	float3 normal = input.normal * 2.0 - 1.0;
	float2 textureCoordinate = input.textureCoordinate * 2.0 - 1.0;
	float2 lightmapCoordinate = input.lightmapCoordinate * 2.0 - 1.0;

	Interstage interstage;
	interstage.position = mul(camera.projectionView, mul(input.worldTransform, float4(position, 1.0)));
	interstage.normal = normal;
	interstage.textureCoordinate = textureCoordinate;
	interstage.lightmapCoordinate = lightmapCoordinate;

	return interstage;
}