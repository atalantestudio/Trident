// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct DirectionalLight {
	float4x4 projectionView;
};

struct VertexInput {
	float3 position : VERTEX_INPUT_ATTRIBUTE0;
	float4x4 worldTransform : VERTEX_INPUT_ATTRIBUTE1;
};

cbuffer DirectionalLight : register(b0) {
	DirectionalLight directionalLight;
}

float4 main(VertexInput input) : SV_Position {
	return mul(directionalLight.projectionView, mul(input.worldTransform, float4(input.position, 1.0)));
}