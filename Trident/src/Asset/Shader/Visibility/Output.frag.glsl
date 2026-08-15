#version 310 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;
precision highp image2D;

layout(set = 0, binding = 0, rgba8) uniform readonly image2D outputImage;

layout(location = 0) out vec4 fragment;

void main() {
	fragment = imageLoad(outputImage, ivec2(gl_FragCoord.xy));
}