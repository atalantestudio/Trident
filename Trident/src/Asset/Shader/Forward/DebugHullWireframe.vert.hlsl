// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint vertexIndex : SV_VertexID;
	float4x4 world : VERTEX_INPUT_ATTRIBUTE0;
	float3x3 normal : VERTEX_INPUT_ATTRIBUTE4;
};

struct VertexOutput {
	float4 position : SV_Position;
};

struct Camera {
	float4x4 projectionView;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

ByteAddressBuffer vertexPositionComponentBuffer : register(t0);

VertexOutput main(VertexInput input) {
	uint positionIndex = input.vertexIndex * 4;
	float3 position = asfloat(vertexPositionComponentBuffer.Load3(positionIndex * 12));

	VertexOutput output;
	output.position = mul(mul(camera.projectionView, input.world), float4(position, 1.0));

	return output;
}