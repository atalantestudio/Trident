// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct FragmentInput {
	float4 position : SV_Position;
	float3 normal : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
};

struct FragmentOutput {
	float4 color : SV_Target;
};

Texture2D<float4> ambientOcclusionRoughnessMetallicTexture : register(t6);
Texture2D<float4> diffuseTexture : register(t7);
Texture2D<float4> normalTexture : register(t8);

SamplerState ambientOcclusionRoughnessMetallicSamplerState : register(s0);
SamplerState diffuseSamplerState : register(s1);
SamplerState normalSamplerState : register(s2);

FragmentOutput main(FragmentInput input) {
	FragmentOutput output;
	output.color = diffuseTexture.Sample(diffuseSamplerState, input.textureCoordinate);

	return output;
}