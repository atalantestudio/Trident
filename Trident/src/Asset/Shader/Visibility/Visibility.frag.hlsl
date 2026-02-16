// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct FragmentInput {
	float4 position : SV_Position;
	nointerpolation uint visibility : VERTEX_OUTPUT_ATTRIBUTE1;
};

struct FragmentOutput {
	uint visibility : SV_Target;
};

FragmentOutput main(FragmentInput input) {
	FragmentOutput output;
	output.visibility = input.visibility;

	return output;
}