// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint3 vertex : VERTEX_INPUT_ATTRIBUTE0;
	float2 lightmapCoordinate : VERTEX_INPUT_ATTRIBUTE1;
	float4x4 worldTransform : VERTEX_INPUT_ATTRIBUTE2;
	float3x3 normalTransform : VERTEX_INPUT_ATTRIBUTE6;
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

StructuredBuffer<float> positionBuffer : register(t0);
StructuredBuffer<float> normalBuffer : register(t1);
StructuredBuffer<float> textureCoordinateBuffer : register(t2);

Interstage main(VertexInput input) {
	uint positionOffset = input.vertex[0] * 3;
	uint normalOffset = input.vertex[1] * 3;
	uint textureCoordinateOffset = input.vertex[2] * 2;

	float3 position = float3(positionBuffer[positionOffset + 0], positionBuffer[positionOffset + 1], positionBuffer[positionOffset + 2]);
	float3 normal = float3(normalBuffer[normalOffset + 0], normalBuffer[normalOffset + 1], normalBuffer[normalOffset + 2]);
	float2 textureCoordinate = float2(textureCoordinateBuffer[textureCoordinateOffset + 0], textureCoordinateBuffer[textureCoordinateOffset + 1]);

	Interstage interstage;
	interstage.position = mul(camera.projectionView, mul(input.worldTransform, float4(position, 1.0)));
	interstage.normal = normal;
	interstage.textureCoordinate = textureCoordinate;
	interstage.lightmapCoordinate = input.lightmapCoordinate;

	return interstage;
}