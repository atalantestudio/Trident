#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

in Interstage {
	layout(location = 0) vec3 surface;
} interstage;

layout(location = 0) out vec4 fragment;

void main() {
	fragment.rgb = vec3(0.0, 0.0, 0.0);
	fragment.a = 0.7;
}