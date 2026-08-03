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
	float3x3 tbn : VERTEX_OUTPUT_ATTRIBUTE3;
};

struct Camera {
	float4x4 projectionView;
	float3 position;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

StructuredBuffer<float> positionBuffer : register(t0);
StructuredBuffer<float> normalBuffer : register(t1);
StructuredBuffer<float> textureCoordinateBuffer : register(t2);
StructuredBuffer<float> tangentBuffer : register(t3);

Interstage main(VertexInput input) {
	uint positionOffset = input.vertex[0] * 3;
	uint normalOffset = input.vertex[1] * 3;
	uint textureCoordinateOffset = input.vertex[2] * 2;
	uint tangentOffset = input.vertexIndex / 3 * 3;

	float3 position = float3(positionBuffer[positionOffset + 0], positionBuffer[positionOffset + 1], positionBuffer[positionOffset + 2]);
	float3 normal = float3(normalBuffer[normalOffset + 0], normalBuffer[normalOffset + 1], normalBuffer[normalOffset + 2]);
	float2 textureCoordinate = float2(textureCoordinateBuffer[textureCoordinateOffset + 0], textureCoordinateBuffer[textureCoordinateOffset + 1]);
	float3 tangent = float3(tangentBuffer[tangentOffset + 0], tangentBuffer[tangentOffset + 1], tangentBuffer[tangentOffset + 2]);

	float4 surface = mul(input.world, float4(position, 1.0));

	float3 n = normalize(mul(input.normal, normal));
	float3 t = normalize(mul(input.normal, tangent));
	float3 b = cross(n, t);

	Interstage interstage;
	interstage.position = mul(camera.projectionView, surface);
	interstage.surface = surface.xyz;
	interstage.textureCoordinate = textureCoordinate;
	interstage.textureCoordinate.y = 1.0 - textureCoordinate.y;
	interstage.tbn = transpose(float3x3(t, b, n));

	return interstage;
}