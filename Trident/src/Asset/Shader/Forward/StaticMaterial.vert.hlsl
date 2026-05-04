// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint2 index : VERTEX_INPUT_ATTRIBUTE0;
};

struct VertexOutput {
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

ByteAddressBuffer positionBuffer : register(t0);
ByteAddressBuffer normalBuffer : register(t1);
ByteAddressBuffer textureCoordinateBuffer : register(t2);

VertexOutput main(VertexInput input) {
	uint positionOffset = (input.index[0] & 0x00ff) * 3;
	uint normalOffset = (input.index[0] & 0xff00) * 3;
	uint textureCoordinateOffset = (input.index[1] & 0x00ff) * 2;

	float3 position = asfloat(positionBuffer.Load3(positionOffset * 4));
	float3 normal = asfloat(normalBuffer.Load3(normalOffset * 4));
	float2 textureCoordinate = asfloat(textureCoordinateBuffer.Load2(textureCoordinateOffset * 4));

	VertexOutput output;
	output.position = mul(camera.projectionView, float4(position, 1.0));
	output.normal = normal;
	output.textureCoordinate = textureCoordinate;

	return output;
}