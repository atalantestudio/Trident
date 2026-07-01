// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	float3 contact : VERTEX_INPUT_ATTRIBUTE0;
};

struct Interstage {
	float4 position : SV_Position;
};

struct Camera {
	float4x4 projectionView;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

Interstage main(VertexInput input) {
	Interstage interstage;
	interstage.position = mul(camera.projectionView, float4(input.contact, 1.0));

	return interstage;
}