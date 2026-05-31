// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint vertexIndex : SV_VertexID;
};

struct Interstage {
	float4 position : SV_Position;
	float3 normal : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
};

struct Camera {
	float4x4 projectionView;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

ByteAddressBuffer triangleBuffer : register(t0);
ByteAddressBuffer vertexBuffer : register(t1);
ByteAddressBuffer normalBuffer : register(t2);
ByteAddressBuffer textureCoordinateBuffer : register(t3);

Interstage main(VertexInput input) {
	uint triangleIndex = input.vertexIndex / 3;
	uint triangleOffset = triangleIndex * 7;
	uint triangleVertexIndex = input.vertexIndex - triangleIndex * 3;

	uint positionOffset = triangleBuffer.Load((triangleOffset + triangleVertexIndex) * 4) * 3;
	uint normalOffset = triangleBuffer.Load((triangleOffset + 3) * 4) * 3;
	uint textureCoordinateOffset = triangleBuffer.Load((triangleOffset + 4 + triangleVertexIndex) * 4) * 2;

	float3 vertex = asfloat(vertexBuffer.Load3(positionOffset * 4));
	float3 normal = asfloat(normalBuffer.Load3(normalOffset * 4));
	float2 textureCoordinate = asfloat(textureCoordinateBuffer.Load2(textureCoordinateOffset * 4));

	Interstage interstage;
	interstage.position = mul(camera.projectionView, float4(vertex, 1.0));
	interstage.normal = normal;
	interstage.textureCoordinate = textureCoordinate;

	return interstage;
}