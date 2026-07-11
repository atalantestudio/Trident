#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(set = 0, binding = 0, rgba8) uniform image2D lightmapAtlasImage;

bool empty(vec4 pixel) {
	return any(equal(pixel.rgb, vec3(0.0, 0.0, 0.0)));
}

void dilate(ivec2 coordinate) {
	vec4 pixel = imageLoad(lightmapAtlasImage, coordinate);

	// Horizontal
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2(-1,  0));
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2( 1,  0));

	// Vertical
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2( 0,  1));
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2( 0, -1));

	// Corners
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2(-1,  1));
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2( 1,  1));
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2( 1, -1));
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2(-1, -1));

	imageStore(lightmapAtlasImage, coordinate, pixel);
}

void main() {
	ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
	ivec2 atlasSize = imageSize(lightmapAtlasImage);

	if (coordinate.x >= atlasSize.x || coordinate.y >= atlasSize.y) {
		return;
	}

	dilate(coordinate);
}