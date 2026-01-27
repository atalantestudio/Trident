#version 440

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

in VertexOutput {
	layout(location = 0) vec3 textureCoordinate;
	layout(location = 1) flat vec4 color;
	layout(location = 2) flat float fontSize;
} vertexOutput;

layout(set = 0, binding = 1) uniform sampler2DArray textureArraySampler;

layout(location = 0) out vec4 fragment;

void main() {
	const float MIN_THRESHOLD_MULTIPLIER = 1.0 / 64.0 * -0.04;
	const float MIN_SMOOTHNESS_MULTIPLIER = 1.0 / 64.0 * 0.05;

	uint smallFontSize = uint(vertexOutput.fontSize < 64.0);
	float inverseFontSize = 64.0 - vertexOutput.fontSize;
	float threshold = smallFontSize * inverseFontSize * MIN_THRESHOLD_MULTIPLIER + 0.5;
	float smoothness = smallFontSize * inverseFontSize * MIN_SMOOTHNESS_MULTIPLIER + 0.05;

	float _sample = texture(textureArraySampler, vertexOutput.textureCoordinate).r;
	float _distance = smoothstep(threshold - smoothness, threshold + smoothness, _sample);

	fragment = _distance * vertexOutput.color;
}