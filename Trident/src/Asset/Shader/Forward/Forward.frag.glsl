#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision mediump float;

in FragmentInput {
	layout(location = 0) vec3 normal;
	layout(location = 1) vec2 textureCoordinate;
} fragmentInput;

layout(location = 0) out vec4 fragment;

void main() {
	fragment = vec4(fragmentInput.textureCoordinate, 0.0, 1.0);
}