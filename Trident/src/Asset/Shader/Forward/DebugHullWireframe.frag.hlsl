// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Interstage {
	float4 position : SV_Position;
};

struct FragmentOutput {
	float4 color : SV_Target;
};

FragmentOutput main(Interstage interstage) {
	FragmentOutput output;
	output.color = float4(0.299, 0.922, 0.0, 1.0);

	return output;
}