// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexOutput {
	float4 position : SV_Position;
	nointerpolation uint visibility : VERTEX_OUTPUT_ATTRIBUTE1;
	nointerpolation uint materialIndex : VERTEX_OUTPUT_ATTRIBUTE2;
};

RWByteAddressBuffer materialCountBuffer : register(u1);

uint main(VertexOutput input) : SV_Target {
	uint _;

	materialCountBuffer.InterlockedAdd(input.materialIndex * 4, 1, _);

	return input.visibility;
}