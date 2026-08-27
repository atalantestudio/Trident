// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
};

struct DirectionalLight {
	float4x4 projectionView;
};

struct VertexInput {
	float3 position : VERTEX_INPUT_ATTRIBUTE0;
	float3 normal : VERTEX_INPUT_ATTRIBUTE1;
	float4 tangent : VERTEX_INPUT_ATTRIBUTE2;
	float2 textureCoordinate : VERTEX_INPUT_ATTRIBUTE3;
	float2 lightmapCoordinate : VERTEX_INPUT_ATTRIBUTE4;
	float4x4 worldTransform : VERTEX_INPUT_ATTRIBUTE5;
	float3x3 normalTransform : VERTEX_INPUT_ATTRIBUTE9;
};

struct Interstage {
	float4 position : SV_Position;
	float3 surface : VERTEX_OUTPUT_ATTRIBUTE1;
	float3x3 tbn : VERTEX_OUTPUT_ATTRIBUTE2;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE5;
	float4 lightSpacePosition : VERTEX_OUTPUT_ATTRIBUTE6;
	float2 lightmapCoordinate : VERTEX_OUTPUT_ATTRIBUTE7;
};

cbuffer Camera {
	Camera camera;
}

cbuffer DirectionalLight {
	DirectionalLight directionalLight;
}

Interstage main(VertexInput input) {
	float4 surface = mul(input.worldTransform, float4(input.position, 1.0));
	float3 n = normalize(mul(input.normalTransform, input.normal * 2.0 - 1.0));
	float3 t = normalize(mul(input.normalTransform, input.tangent.xyz * 2.0 - 1.0));
	float3 b = cross(n, t);

	Interstage interstage;
	interstage.position = mul(camera.projectionView, surface);
	interstage.surface = surface.xyz;
	interstage.tbn = float3x3(t, b, n);
	interstage.textureCoordinate = input.textureCoordinate;
	interstage.lightSpacePosition = mul(directionalLight.projectionView, surface);
	interstage.lightmapCoordinate = input.lightmapCoordinate;

	return interstage;
}