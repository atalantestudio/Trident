// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	float3 position : VERTEX_INPUT_ATTRIBUTE0;
	float3 normal : VERTEX_INPUT_ATTRIBUTE1;
	float3 tangent : VERTEX_INPUT_ATTRIBUTE2;
	float2 textureCoordinate : VERTEX_INPUT_ATTRIBUTE3;
	float4x4 worldTransform : VERTEX_INPUT_ATTRIBUTE4;
	float3x3 normalTransform : VERTEX_INPUT_ATTRIBUTE8;
};

struct Interstage {
	float4 position : SV_Position;
	float3 surface : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
	float3x3 tbn : VERTEX_OUTPUT_ATTRIBUTE3;
};

struct Camera {
	float4x4 projectionView;
	float3 position;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

Interstage main(VertexInput input) {
	float3 normal = input.normal * 2.0 - 1.0;
	float3 tangent = input.tangent * 2.0 - 1.0;
	float2 textureCoordinate = input.textureCoordinate * 2.0 - 1.0;
	float4 surface = mul(input.worldTransform, float4(input.position, 1.0));

	float3 n = normalize(mul(input.normalTransform, normal));
	float3 t = normalize(mul(input.normalTransform, tangent));
	float3 b = cross(n, t);

	Interstage interstage;
	interstage.position = mul(camera.projectionView, surface);
	interstage.surface = surface.xyz;
	interstage.textureCoordinate = textureCoordinate;
	interstage.textureCoordinate.y = 1.0 - textureCoordinate.y;
	interstage.tbn = transpose(float3x3(t, b, n));

	return interstage;
}