// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	float4 vertexAndTextureCoordinate : VERTEX_INPUT_ATTRIBUTE0;
	float3x3 worldTransform : VERTEX_INPUT_ATTRIBUTE1;
	float3x3 textureTransform : VERTEX_INPUT_ATTRIBUTE4;
	float4 color : VERTEX_INPUT_ATTRIBUTE7;
	uint zIndex : VERTEX_INPUT_ATTRIBUTE8;
	uint layerIndex : VERTEX_INPUT_ATTRIBUTE9;
};

struct Interstage {
	float4 position : SV_Position;
	nointerpolation float4 color : VERTEX_OUTPUT_ATTRIBUTE1;
	float3 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
};

struct Camera {
	float3x3 projection;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

Interstage main(VertexInput input) {
	float3 vertex = float3(input.vertexAndTextureCoordinate.xy, 1.0);
	float3 textureCoordinate = float3(input.vertexAndTextureCoordinate.zw, 1.0);

	Interstage interstage;
	interstage.position = float4(mul(camera.projection, mul(input.worldTransform, vertex)), 1.0);
	interstage.position.z = float(input.zIndex) / 100.0;
	interstage.color = input.color;
	interstage.textureCoordinate = mul(input.textureTransform, textureCoordinate);
	interstage.textureCoordinate.z = float(input.layerIndex);

	return interstage;
}