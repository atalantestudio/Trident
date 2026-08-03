#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

in Interstage {
	layout(location = 0) vec2 textureCoordinate;
} interstage;

layout(set = 0, binding = 0) uniform sampler2D intermediateLightmapAtlas;

layout(location = 0) out vec4 fragment;

void main() {
	vec4 color = texture(intermediateLightmapAtlas, interstage.textureCoordinate);

	// TODO(asset-builder/lightmapper): Test if post-dilation is useful.
	/* if (color.a == 0.0) color = textureOffset(intermediateLightmapAtlas, interstage.textureCoordinate, ivec2(-1, -1));
	if (color.a == 0.0) color = textureOffset(intermediateLightmapAtlas, interstage.textureCoordinate, ivec2( 0, -1));
	if (color.a == 0.0) color = textureOffset(intermediateLightmapAtlas, interstage.textureCoordinate, ivec2( 1, -1));
	if (color.a == 0.0) color = textureOffset(intermediateLightmapAtlas, interstage.textureCoordinate, ivec2(-1,  0));
	if (color.a == 0.0) color = textureOffset(intermediateLightmapAtlas, interstage.textureCoordinate, ivec2( 1,  0));
	if (color.a == 0.0) color = textureOffset(intermediateLightmapAtlas, interstage.textureCoordinate, ivec2(-1,  1));
	if (color.a == 0.0) color = textureOffset(intermediateLightmapAtlas, interstage.textureCoordinate, ivec2( 0,  1));
	if (color.a == 0.0) color = textureOffset(intermediateLightmapAtlas, interstage.textureCoordinate, ivec2( 1,  1)); */

	fragment = color;
}