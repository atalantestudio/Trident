#version 310 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

layout(location = 0) out vec4 fragment;

void main() {
	fragment = vec4(0.299, 0.922, 0.0, 1.0);
}