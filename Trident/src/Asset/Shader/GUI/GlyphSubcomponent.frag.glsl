#version 320 es

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;
precision highp sampler2DArray;

in FragmentInput {
	layout(location = 0) flat vec4 color;
	layout(location = 1) vec3 textureCoordinate;
	layout(location = 2) flat float fontSize;
} fragmentInput;

const float MIN_THRESHOLD_MULTIPLIER = 1.0 / 64.0 * -0.04;
const float MIN_SMOOTHNESS_MULTIPLIER = 1.0 / 64.0 * 0.05;

layout(set = 0, binding = 1) uniform sampler2DArray textureArray;

layout(location = 0) out vec4 fragment;

void main() {
	float smallFontSize = float(fragmentInput.fontSize < 64.0);
	float inverseFontSize = 64.0 - fragmentInput.fontSize;
	float threshold = smallFontSize * inverseFontSize * MIN_THRESHOLD_MULTIPLIER + 0.5;
	float smoothness = smallFontSize * inverseFontSize * MIN_SMOOTHNESS_MULTIPLIER + 0.05;

	float _sample = texture(textureArray, fragmentInput.textureCoordinate).r;
	float _distance = smoothstep(threshold - smoothness, threshold + smoothness, _sample);

	// TODO(ui/smooth-text): Fix pixelated text at small font sizes.
	/* if (fragmentInput.fontSize == 24.0) {
		_distance = smoothstep(0.3, 0.6, _sample);
	} */

	fragment = fragmentInput.color * _distance;
}