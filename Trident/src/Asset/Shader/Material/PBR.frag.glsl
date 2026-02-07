#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision mediump float;

in FragmentInput {
	layout(location = 0) vec3 normal;
	layout(location = 1) vec2 textureCoordinate;
} fragmentInput;

layout(set = 1, binding = 0) uniform sampler2D ambientOcclusionRoughnessMetallicTexture;
layout(set = 1, binding = 1) uniform sampler2D diffuseTexture;
layout(set = 1, binding = 2) uniform sampler2D normalTexture;

layout(location = 0) out vec4 fragment;

void main() {
	fragment = texture(diffuseTexture, fragmentInput.textureCoordinate);
}