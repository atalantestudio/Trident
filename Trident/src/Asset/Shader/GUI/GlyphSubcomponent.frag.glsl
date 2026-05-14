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

layout(set = 0, binding = 1) uniform sampler2D userImages[8];

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
	float antialiasPerEm = interstage.fontSize;
	float _sample = 0.0;

	switch (uint(interstage.textureCoordinate.z)) {
		case 0: _sample = texture(userImages[0], interstage.textureCoordinate.xy).r; break;
		case 1: _sample = texture(userImages[1], interstage.textureCoordinate.xy).r; break;
		case 2: _sample = texture(userImages[2], interstage.textureCoordinate.xy).r; break;
		case 3: _sample = texture(userImages[3], interstage.textureCoordinate.xy).r; break;
		case 4: _sample = texture(userImages[4], interstage.textureCoordinate.xy).r; break;
		case 5: _sample = texture(userImages[5], interstage.textureCoordinate.xy).r; break;
		case 6: _sample = texture(userImages[6], interstage.textureCoordinate.xy).r; break;
		case 7: _sample = texture(userImages[7], interstage.textureCoordinate.xy).r; break;
	}

	float distanceEm = mix(0.05, -0.05, _sample);

	// NOTE: Looks good when multiplied by something larger than 2.
	float inverseWidth = screenScale * antialiasPerEm * 2.5;

	float opacity = clamp(-distanceEm * inverseWidth + 0.5, 0.0, 1.0);

	fragment = interstage.color * opacity;
}