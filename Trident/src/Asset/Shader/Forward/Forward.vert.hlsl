// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint vertexIndex : SV_VertexID;
	float4x4 transform : VERTEX_INPUT_ATTRIBUTE0;
	float3x3 normal : VERTEX_INPUT_ATTRIBUTE4;
};

struct VertexOutput {
	float4 position : SV_Position;
	float3 normal : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
	float3 surface : VERTEX_OUTPUT_ATTRIBUTE3;
	float3x3 instanceNormal : VERTEX_OUTPUT_ATTRIBUTE4;
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

VertexOutput main(VertexInput input) {
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

	VertexOutput output;
	output.position = mul(mul(camera.projectionView, input.transform), float4(position, 1.0));
	output.normal = normal;
	output.textureCoordinate = textureCoordinate;
	output.textureCoordinate.y = 1.0 - output.textureCoordinate.y;
	output.surface = position.rgb;
	output.instanceNormal = input.normal;

	return output;
}