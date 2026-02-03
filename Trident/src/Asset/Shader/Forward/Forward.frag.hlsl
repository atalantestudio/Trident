// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct FragmentInput {
	float4 position : SV_Position;
};

struct FragmentOutput {
	float4 color : SV_Target;
};

FragmentOutput main() {
	FragmentOutput output;
	output.color = float4(1.0, 1.0, 1.0, 1.0);

	return output;
}