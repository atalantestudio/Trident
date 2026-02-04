// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct FragmentInput {
	float4 position : SV_Position;
};

struct FragmentOutput {
	float4 color : SV_Target;
};

Texture2D<float4> ambientOcclusionRoughnessMetallicTexture;
Texture2D<float4> diffuseTexture;
Texture2D<float4> normalTexture;

FragmentOutput main(FragmentInput input) {
	FragmentOutput output;
	output.color = float4(1.0, 0.2, 0.0, 1.0);

	return output;
}