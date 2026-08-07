// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Interstage {
	float4 position : SV_Position;
	float3 normal : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
	float2 lightmapCoordinate : VERTEX_OUTPUT_ATTRIBUTE3;
};

struct FragmentOutput {
	float4 color : SV_Target;
};

struct Camera {
	float4x4 projectionView;
	float3 position;
};

struct Light {
	float3 position;
	float3 color;
};

Light createLight(float3 position, float3 color) {
	Light light;
	light.position = position;
	light.color = color;

	return light;
}

static const uint LIGHT_COUNT = 2;
static const Light LIGHTS[LIGHT_COUNT] = {
	createLight(float3(-128.0, 128.0, -128.0), float3(300000.0, 300000.0, 300000.0)),
	createLight(float3(128.0, 128.0, 128.0), float3(300000.0, 300000.0, 300000.0)),
};

cbuffer Camera : register(b0) {
	Camera camera;
}

Texture2D<float4> lightmapAtlasTexture : register(t0);
SamplerState lightmapAtlasSamplerState : register(s0);

Texture2D<float4> armTexture : register(t1);
SamplerState armSamplerState : register(s1);

Texture2D<float4> diffuseTexture : register(t2);
SamplerState diffuseSamplerState : register(s2);

Texture2D<float4> normalTexture : register(t3);
SamplerState normalSamplerState : register(s3);

FragmentOutput main(Interstage interstage) {
	// Convert from sRGB space to linear space.
	float3 diffuse = pow(diffuseTexture.Sample(diffuseSamplerState, interstage.textureCoordinate).rgb, float3(2.2, 2.2, 2.2));

	// TODO(forward/static): Sample the lightmap atlas with bicubic interpolation.
	float3 light = lightmapAtlasTexture.Sample(lightmapAtlasSamplerState, interstage.lightmapCoordinate).rgb;

	float3 arm = armTexture.Sample(armSamplerState, interstage.textureCoordinate).rgb;
	float ao = arm[0];
	float roughness = arm[1];

	FragmentOutput output;
	output.color.rgb = diffuse * light * ao;
	output.color.rgb = pow(output.color.rgb, 1.0 / 2.2);
	output.color.a = 1.0;

	return output;
}