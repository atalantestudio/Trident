#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(set = 0, binding = 0, rgba8) uniform image2D intermediateLightmapAtlas;

void loadLuxel(ivec2 coordinate, ivec2 atlasSize, inout vec4 color) {
	if (coordinate.x < 0 || coordinate.x >= atlasSize.x) {
		return;
	}

	if (coordinate.y < 0 || coordinate.y >= atlasSize.y) {
		return;
	}

	color = imageLoad(intermediateLightmapAtlas, coordinate);
}

void main() {
	ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
	ivec2 atlasSize = imageSize(intermediateLightmapAtlas);

	if (coordinate.x >= atlasSize.x || coordinate.y >= atlasSize.y) {
		return;
	}

	vec4 color = imageLoad(intermediateLightmapAtlas, coordinate);

	if (color.a == 0.0) loadLuxel(coordinate + ivec2( 0,  1), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + ivec2( 1,  0), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + ivec2( 0, -1), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + ivec2(-1,  0), atlasSize, color);

	if (color.a == 0.0) loadLuxel(coordinate + ivec2(-1,  1), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + ivec2( 1,  1), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + ivec2( 1, -1), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + ivec2(-1, -1), atlasSize, color);

	imageStore(intermediateLightmapAtlas, coordinate, color);
}