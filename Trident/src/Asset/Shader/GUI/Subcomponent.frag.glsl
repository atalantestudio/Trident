#version 320 es

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;
precision highp sampler2DArray;

in FragmentInput {
	layout(location = 0) flat vec4 color;
	layout(location = 1) vec3 textureCoordinate;
} fragmentInput;

layout(set = 0, binding = 1) uniform sampler2DArray textureArray;

layout(location = 0) out vec4 fragment;

void main() {
	fragment = fragmentInput.color * texture(textureArray, fragmentInput.textureCoordinate);
}