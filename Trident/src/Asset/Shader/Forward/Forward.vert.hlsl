// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint vertexIndex : SV_VertexID;
	uint3 vertex : VERTEX_INPUT_ATTRIBUTE0;
	float4x4 world : VERTEX_INPUT_ATTRIBUTE1;
	float3x3 normal : VERTEX_INPUT_ATTRIBUTE5;
};

struct VertexOutput {
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

ByteAddressBuffer vertexPositionComponentBuffer : register(t0);
ByteAddressBuffer vertexNormalComponentBuffer : register(t1);
ByteAddressBuffer vertexTextureCoordinateComponentBuffer : register(t2);
ByteAddressBuffer triangleTangentComponentBuffer : register(t3);

VertexOutput main(VertexInput input) {
	uint positionComponentOffset = input.vertex[0] * 3;
	uint normalComponentOffset = input.vertex[1] * 3;
	uint textureCoordinateComponentOffset = input.vertex[2] * 2;
	uint tangentComponentOffset = input.vertexIndex / 3 * 3;

	float3 position = asfloat(vertexPositionComponentBuffer.Load3(positionComponentOffset * 4));
	float3 normal = asfloat(vertexNormalComponentBuffer.Load3(normalComponentOffset * 4));
	float2 textureCoordinate = asfloat(vertexTextureCoordinateComponentBuffer.Load2(textureCoordinateComponentOffset * 4));
	float3 tangent = asfloat(triangleTangentComponentBuffer.Load3(tangentComponentOffset * 4));

	float4 surface = mul(input.world, float4(position, 1.0));

	float3 N = normalize(mul(input.normal, normal));
	float3 T = normalize(mul(input.normal, tangent));
	float3 B = cross(N, T);

	VertexOutput output;
	output.position = mul(camera.projectionView, surface);
	output.surface = surface.xyz;
	output.textureCoordinate = float2(textureCoordinate.x, 1.0 - textureCoordinate.y);
	output.TBN = transpose(float3x3(T, B, N));

	return output;
}