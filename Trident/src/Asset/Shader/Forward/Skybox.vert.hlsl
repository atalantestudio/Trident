// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
	float3 position;
	uint pointLightCount;
	float3 ambientColor;
	uint _0;
	float3 sunColor;
	uint _1;
	float4x4 skyboxProjectionView;
};

struct Interstage {
	float4 position : SV_Position;
	float3 direction : VERTEX_OUTPUT_ATTRIBUTE1;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

static const float3 POSITIONS[36] = {
	// Front
	float3(-0.5,  0.5,  0.5),
	float3( 0.5,  0.5,  0.5),
	float3(-0.5, -0.5,  0.5),
	float3( 0.5,  0.5,  0.5),
	float3( 0.5, -0.5,  0.5),
	float3(-0.5, -0.5,  0.5),

	// Back
	float3( 0.5,  0.5, -0.5),
	float3(-0.5,  0.5, -0.5),
	float3( 0.5, -0.5, -0.5),
	float3(-0.5,  0.5, -0.5),
	float3(-0.5, -0.5, -0.5),
	float3( 0.5, -0.5, -0.5),

	// Left
	float3(-0.5,  0.5, -0.5),
	float3(-0.5,  0.5,  0.5),
	float3(-0.5, -0.5, -0.5),
	float3(-0.5,  0.5,  0.5),
	float3(-0.5, -0.5,  0.5),
	float3(-0.5, -0.5, -0.5),

	// Right
	float3( 0.5,  0.5,  0.5),
	float3( 0.5,  0.5, -0.5),
	float3( 0.5, -0.5,  0.5),
	float3( 0.5,  0.5, -0.5),
	float3( 0.5, -0.5, -0.5),
	float3( 0.5, -0.5,  0.5),

	// Top
	float3(-0.5,  0.5, -0.5),
	float3( 0.5,  0.5, -0.5),
	float3(-0.5,  0.5,  0.5),
	float3( 0.5,  0.5, -0.5),
	float3( 0.5,  0.5,  0.5),
	float3(-0.5,  0.5,  0.5),

	// Bottom
	float3(-0.5, -0.5,  0.5),
	float3( 0.5, -0.5,  0.5),
	float3(-0.5, -0.5, -0.5),
	float3( 0.5, -0.5,  0.5),
	float3( 0.5, -0.5, -0.5),
	float3(-0.5, -0.5, -0.5),
};

Interstage main(uint vertexIndex : SV_VertexID) {
	float3 position = POSITIONS[vertexIndex];

	Interstage interstage;
	interstage.position = mul(camera.skyboxProjectionView, float4(position, 1.0));
	interstage.direction = position;

	return interstage;
}