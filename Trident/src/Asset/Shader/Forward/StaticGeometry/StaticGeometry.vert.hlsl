// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	float2 lightmapCoordinate : VERTEX_INPUT_ATTRIBUTE0;
	uint4 index : VERTEX_INPUT_ATTRIBUTE1;
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

StructuredBuffer<float> vertexBuffer : register(t0);
StructuredBuffer<float> normalBuffer : register(t1);
StructuredBuffer<float> textureCoordinateBuffer : register(t2);

Interstage main(VertexInput input) {
	uint vertexIndex = input.index[0];
	uint normalIndex = input.index[1];
	uint textureCoordinateIndex = input.index[2];

	float3 vertex = float3(vertexBuffer[vertexIndex * 3 + 0], vertexBuffer[vertexIndex * 3 + 1], vertexBuffer[vertexIndex * 3 + 2]);
	float3 normal = float3(normalBuffer[normalIndex * 3 + 0], normalBuffer[normalIndex * 3 + 1], normalBuffer[normalIndex * 3 + 2]);
	float2 textureCoordinate = float2(textureCoordinateBuffer[textureCoordinateIndex * 2 + 0], textureCoordinateBuffer[textureCoordinateIndex * 2 + 1]);

	Interstage interstage;
	interstage.position = mul(camera.projectionView, float4(vertex, 1.0));
	interstage.normal = normal;
	interstage.textureCoordinate = textureCoordinate;
	interstage.lightmapCoordinate = input.lightmapCoordinate;

	return interstage;
}