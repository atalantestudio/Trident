#version 310 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision mediump float;

layout(set = 1, binding = 0) uniform sampler2D ambientOcclusionRoughnessMetallicTexture;
layout(set = 1, binding = 1) uniform sampler2D diffuseTexture;
layout(set = 1, binding = 2) uniform sampler2D normalTexture;

layout(location = 0) out vec4 fragment;

void main() {
	fragment = vec4(1.0, 0.2, 0.0, 1.0);
}