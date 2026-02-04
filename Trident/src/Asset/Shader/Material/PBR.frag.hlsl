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

Texture2D<float4> ambientOcclusionRoughnessMetallicTexture;
Texture2D<float4> diffuseTexture;
Texture2D<float4> normalTexture;

FragmentOutput main(FragmentInput input) {
	FragmentOutput output;
	output.color = float4(input.textureCoordinate, 0.0, 1.0);

	return output;
}