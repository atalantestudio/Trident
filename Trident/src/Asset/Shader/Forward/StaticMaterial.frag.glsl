#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

in Interstage {
	layout(location = 0) vec3 normal;
	layout(location = 1) vec2 textureCoordinate;
	layout(location = 2) vec2 lightmapCoordinate;
} interstage;

layout(set = 0, binding = 6) uniform sampler2D lightmapAtlasSampler;

layout(location = 0) out vec4 fragment;

void main() {
	fragment = texture(lightmapAtlasSampler, interstage.lightmapCoordinate);
}