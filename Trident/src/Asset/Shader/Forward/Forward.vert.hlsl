// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	float4x4 transform : VERTEX_INPUT_ATTRIBUTE0;
	float3 position : VERTEX_INPUT_ATTRIBUTE4;
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

VertexOutput main(VertexInput input) {
	VertexOutput output;
	output.position = mul(mul(camera.projectionView, input.transform), float4(input.position, 1.0));

	return output;
}