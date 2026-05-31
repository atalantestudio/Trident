// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Interstage {
	float4 position : SV_Position;
	float3 normal : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
};

struct FragmentOutput {
	float4 color : SV_Target;
};

FragmentOutput main(Interstage interstage) {
	FragmentOutput output;
	output.color = float4(interstage.textureCoordinate, 0.0, 1.0);

	return output;
}