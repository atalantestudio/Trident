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
ByteAddressBuffer vertexPositionIndexBuffer : register(t1);
ByteAddressBuffer vertexNormalComponentBuffer : register(t2);
ByteAddressBuffer vertexNormalIndexBuffer : register(t3);
ByteAddressBuffer vertexTextureCoordinateComponentBuffer : register(t4);
ByteAddressBuffer vertexTextureCoordinateIndexBuffer : register(t5);
ByteAddressBuffer vertexTangentComponentBuffer : register(t6);

VertexOutput main(VertexInput input) {
	uint triangleIndex = input.vertexIndex / 3;
	uint positionIndex = vertexPositionIndexBuffer.Load(input.vertexIndex * 4);
	uint normalIndex = vertexNormalIndexBuffer.Load(input.vertexIndex * 4);
	uint textureCoordinateIndex = vertexTextureCoordinateIndexBuffer.Load(input.vertexIndex * 4);

	float3 position = float3(
		asfloat(vertexPositionComponentBuffer.Load((positionIndex * 3 + 0) * 4)),
		asfloat(vertexPositionComponentBuffer.Load((positionIndex * 3 + 1) * 4)),
		asfloat(vertexPositionComponentBuffer.Load((positionIndex * 3 + 2) * 4))
	);

	float3 normal = float3(
		asfloat(vertexNormalComponentBuffer.Load((normalIndex * 3 + 0) * 4)),
		asfloat(vertexNormalComponentBuffer.Load((normalIndex * 3 + 1) * 4)),
		asfloat(vertexNormalComponentBuffer.Load((normalIndex * 3 + 2) * 4))
	);

	float2 textureCoordinate = float2(
		asfloat(vertexTextureCoordinateComponentBuffer.Load((textureCoordinateIndex * 2 + 0) * 4)),
		asfloat(vertexTextureCoordinateComponentBuffer.Load((textureCoordinateIndex * 2 + 1) * 4))
	);

	float3 tangent = float3(
		asfloat(vertexTangentComponentBuffer.Load((triangleIndex * 3 + 0) * 4)),
		asfloat(vertexTangentComponentBuffer.Load((triangleIndex * 3 + 1) * 4)),
		asfloat(vertexTangentComponentBuffer.Load((triangleIndex * 3 + 2) * 4))
	);

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