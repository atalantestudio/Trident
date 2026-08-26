#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

in Interstage {
	layout(location = 0) vec3 direction;
} interstage;

layout(set = 0, binding = 1) uniform samplerCube skybox;

layout(location = 0) out vec4 fragment;

void main() {
	fragment = texture(skybox, interstage.direction);
}