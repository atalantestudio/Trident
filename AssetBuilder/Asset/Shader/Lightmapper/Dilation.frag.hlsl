// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Interstage {
	float4 position : SV_Position;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE1;
};

struct FragmentOutput {
	float4 color : SV_Target;
};

Texture2D<float4> intermediateLightmapAtlas : register(t0);
SamplerState intermediateLightmapAtlasSampler : register(s0);

FragmentOutput main(Interstage interstage) {
	float4 color = intermediateLightmapAtlas.Sample(intermediateLightmapAtlasSampler, interstage.textureCoordinate);

	if (color.a == 0.0) color = intermediateLightmapAtlas.Sample(intermediateLightmapAtlasSampler, interstage.textureCoordinate, int2(-1, -1));
	if (color.a == 0.0) color = intermediateLightmapAtlas.Sample(intermediateLightmapAtlasSampler, interstage.textureCoordinate, int2( 0, -1));
	if (color.a == 0.0) color = intermediateLightmapAtlas.Sample(intermediateLightmapAtlasSampler, interstage.textureCoordinate, int2( 1, -1));
	if (color.a == 0.0) color = intermediateLightmapAtlas.Sample(intermediateLightmapAtlasSampler, interstage.textureCoordinate, int2(-1,  0));
	if (color.a == 0.0) color = intermediateLightmapAtlas.Sample(intermediateLightmapAtlasSampler, interstage.textureCoordinate, int2( 1,  0));
	if (color.a == 0.0) color = intermediateLightmapAtlas.Sample(intermediateLightmapAtlasSampler, interstage.textureCoordinate, int2(-1,  1));
	if (color.a == 0.0) color = intermediateLightmapAtlas.Sample(intermediateLightmapAtlasSampler, interstage.textureCoordinate, int2( 0,  1));
	if (color.a == 0.0) color = intermediateLightmapAtlas.Sample(intermediateLightmapAtlasSampler, interstage.textureCoordinate, int2( 1,  1));

	FragmentOutput output;
	output.color = color;

	return output;
}