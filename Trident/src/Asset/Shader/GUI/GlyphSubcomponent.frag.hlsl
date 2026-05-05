// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexOutput {
	float4 position : SV_Position;
	nointerpolation float4 color : VERTEX_OUTPUT_ATTRIBUTE1;
	float3 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
	nointerpolation float fontSize : VERTEX_OUTPUT_ATTRIBUTE3;
};

Texture2DArray<float4> imageArray : register(t0);
SamplerState imageArraySamplerState : register(s0);

float4 main(VertexOutput input) : SV_Target {
	float midPoint = 0.5 - 1.2 / input.fontSize;
	float width = 3.5 / input.fontSize + 0.02;

	float _sample = imageArray.Sample(imageArraySamplerState, input.textureCoordinate).r;
	float distance = smoothstep(midPoint - width, midPoint + width, _sample);

	return distance * input.color;
}