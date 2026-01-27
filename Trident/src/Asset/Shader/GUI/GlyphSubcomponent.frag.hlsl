// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexOutput {
	float4 position : SV_Position;
	float3 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE1;
	nointerpolation float4 color : VERTEX_OUTPUT_ATTRIBUTE2;
	nointerpolation float fontSize : VERTEX_OUTPUT_ATTRIBUTE3;
};

static const float MIN_THRESHOLD_MULTIPLIER = 1.0 / 64.0 * -0.04;
static const float MIN_SMOOTHNESS_MULTIPLIER = 1.0 / 64.0 * 0.05;

Texture2DArray<float4> imageArray : register(t0);
SamplerState imageArraySamplerState : register(s0);

float4 main(VertexOutput input) : SV_Target {
	uint smallFontSize = input.fontSize < 64.0;
	float inverseFontSize = 64.0 - input.fontSize;
	float threshold = smallFontSize * inverseFontSize * MIN_THRESHOLD_MULTIPLIER + 0.5;
	float smoothness = smallFontSize * inverseFontSize * MIN_SMOOTHNESS_MULTIPLIER + 0.05;

	float _sample = imageArray.Sample(imageArraySamplerState, input.textureCoordinate).r;
	float distance = smoothstep(threshold - smoothness, threshold + smoothness, _sample);

	return distance * input.color;
}