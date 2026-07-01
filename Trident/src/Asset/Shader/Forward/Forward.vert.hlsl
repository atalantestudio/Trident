// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint vertexIndex : SV_VertexID;
	uint3 vertex : VERTEX_INPUT_ATTRIBUTE0;
	float4x4 world : VERTEX_INPUT_ATTRIBUTE1;
	float3x3 normal : VERTEX_INPUT_ATTRIBUTE5;
};

struct Interstage {
	float4 position : SV_Position;
	float3 surface : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
	float3x3 TBN : VERTEX_OUTPUT_ATTRIBUTE3;
};

struct Camera {
	float4x4 projectionView;
	float3 position;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

StructuredBuffer<float> vertexPositionComponentBuffer : register(t0);
StructuredBuffer<float> vertexNormalComponentBuffer : register(t1);
StructuredBuffer<float> vertexTextureCoordinateComponentBuffer : register(t2);
StructuredBuffer<float> triangleTangentComponentBuffer : register(t3);

Interstage main(VertexInput input) {
	uint positionComponentOffset = input.vertex[0] * 3;
	uint normalComponentOffset = input.vertex[1] * 3;
	uint textureCoordinateComponentOffset = input.vertex[2] * 2;
	uint tangentComponentOffset = input.vertexIndex / 3 * 3;

	float3 position = float3(
		vertexPositionComponentBuffer[positionComponentOffset + 0],
		vertexPositionComponentBuffer[positionComponentOffset + 1],
		vertexPositionComponentBuffer[positionComponentOffset + 2]
	);
	float3 normal = float3(
		vertexNormalComponentBuffer[normalComponentOffset + 0],
		vertexNormalComponentBuffer[normalComponentOffset + 1],
		vertexNormalComponentBuffer[normalComponentOffset + 2]
	);
	float2 textureCoordinate = float2(
		vertexTextureCoordinateComponentBuffer[textureCoordinateComponentOffset + 0],
		vertexTextureCoordinateComponentBuffer[textureCoordinateComponentOffset + 1]
	);
	float3 tangent = float3(
		triangleTangentComponentBuffer[tangentComponentOffset + 0],
		triangleTangentComponentBuffer[tangentComponentOffset + 1],
		triangleTangentComponentBuffer[tangentComponentOffset + 2]
	);

	float4 surface = mul(input.world, float4(position, 1.0));

	float3 N = normalize(mul(input.normal, normal));
	float3 T = normalize(mul(input.normal, tangent));
	float3 B = cross(N, T);

	Interstage interstage;
	interstage.position = mul(camera.projectionView, surface);
	interstage.surface = surface.xyz;
	interstage.textureCoordinate = float2(textureCoordinate.x, 1.0 - textureCoordinate.y);
	interstage.TBN = transpose(float3x3(T, B, N));

	return interstage;
}