// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Interstage {
	float4 position : SV_Position;
	float3 direction : VERTEX_OUTPUT_ATTRIBUTE1;
};

TextureCube<float4> skyboxTexture : register(t0);
SamplerState skyboxSamplerState : register(s0);

float4 main(Interstage interstage) : SV_Target0 {
	return skyboxTexture.Sample(skyboxSamplerState, interstage.direction);
}