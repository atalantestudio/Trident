#version 320 es

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;
precision highp sampler2DArray;

in Interstage {
	layout(location = 0) flat vec4 color;
	layout(location = 1) vec3 textureCoordinate;
	layout(location = 2) flat float fontSize;

	// TODO(gui/text-smoothness)
	// layout(location = 3) float a;
	// layout(location = 4) float b;
} interstage;

layout(set = 0, binding = 1) uniform sampler2DArray textureArray;

layout(location = 0) out vec4 fragment;

// See https://www.redblobgames.com/articles/sdf-fonts.
float calculateScreenScalePx() {
	const vec2 atlasSize = vec2(2048.0, 2048.0);

	vec2 gradient = fwidth(interstage.textureCoordinate.xy);
	vec2 product = atlasSize * gradient;

	return max(0.5 * dot(atlasSize, gradient) / (product.x * product.y), 1.0);
}

void main() {
	float screenScale = calculateScreenScalePx();

	// NOTE: Looks good when multiplied by 3.0.
	float antialiasPerEm = interstage.fontSize * 3.0;

	float _sample = texture(textureArray, interstage.textureCoordinate).r;
	float distanceEm = mix(0.05, -0.05, _sample);
	float inverseWidth = screenScale * antialiasPerEm;
	float opacity = clamp(-distanceEm * inverseWidth + 0.5, 0.0, 1.0);

	fragment = interstage.color * opacity;
}