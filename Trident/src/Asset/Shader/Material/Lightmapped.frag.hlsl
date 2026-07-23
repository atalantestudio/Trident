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

Texture2D<float4> lightmapAtlasTexture : register(t5);
Texture2D<float4> diffuseTexture : register(t6);
SamplerState lightmapAtlasSamplerState : register(s0);
SamplerState diffuseSamplerState : register(s1);

FragmentOutput main(Interstage interstage) {
	float3 diffuse = diffuseTexture.Sample(diffuseSamplerState, interstage.textureCoordinate).rgb;

	// TODO(forward/static): Sample the lightmap atlas with bicubic interpolation.
	float3 light = lightmapAtlasTexture.Sample(lightmapAtlasSamplerState, interstage.lightmapCoordinate).rgb;

	FragmentOutput output;
	output.color.rgb = diffuse * light;

	output.color.rgb = pow(output.color.rgb, 1.0 / 2.2);
	output.color.a = 1.0;

	// TODO(dx12): Fix some materials not being rendered.
	return output;
}