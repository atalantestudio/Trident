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

void main() {
	float midPoint = 0.5 - 1.2 / interstage.fontSize;
	float width = 3.5 / interstage.fontSize + 0.02;

	float _sample = texture(textureArray, interstage.textureCoordinate).r;
	float _distance = smoothstep(midPoint - width, midPoint + width, _sample);

	fragment = interstage.color * _distance;
}