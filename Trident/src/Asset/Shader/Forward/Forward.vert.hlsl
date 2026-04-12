// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint vertexIndex : SV_VertexID;
	float4x4 transform : VERTEX_INPUT_ATTRIBUTE0;
	float3x3 normal : VERTEX_INPUT_ATTRIBUTE4;
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
ByteAddressBuffer vertexPositionIndexBuffer : register(t4);
ByteAddressBuffer vertexNormalIndexBuffer : register(t5);
ByteAddressBuffer vertexTextureCoordinateIndexBuffer : register(t6);

VertexOutput main(VertexInput input) {
	uint triangleIndex = input.vertexIndex / 3;
	uint positionIndex = vertexPositionIndexBuffer.Load(input.vertexIndex * 4);
	uint normalIndex = vertexNormalIndexBuffer.Load(input.vertexIndex * 4);
	uint textureCoordinateIndex = vertexTextureCoordinateIndexBuffer.Load(input.vertexIndex * 4);

	float3 position = asfloat(vertexPositionComponentBuffer.Load3(positionIndex * 12));
	float3 normal = asfloat(vertexNormalComponentBuffer.Load3(normalIndex * 12));
	float2 textureCoordinate = asfloat(vertexTextureCoordinateComponentBuffer.Load2(textureCoordinateIndex * 8));
	float3 tangent = asfloat(triangleTangentComponentBuffer.Load3(triangleIndex * 12));

	float4 surface = mul(input.transform, float4(position, 1.0));

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