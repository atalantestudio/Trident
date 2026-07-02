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

Texture2D<float4> lightmapAtlasImage : register(t5);
SamplerState lightmapAtlasSamplerState : register(s0);

FragmentOutput main(Interstage interstage) {
	FragmentOutput output;
	output.color = lightmapAtlasImage.Sample(lightmapAtlasSamplerState, interstage.lightmapCoordinate);

	return output;
}