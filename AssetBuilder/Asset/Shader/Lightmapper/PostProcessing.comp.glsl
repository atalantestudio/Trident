#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

const uvec2 ATLAS_SIZE = uvec2(2048, 2048);

layout(set = 0, binding = 0, rgba8) uniform image2D lightmapAtlasImage;

bool empty(vec4 pixel) {
	return dot(pixel.rgb, pixel.rgb) == 0.0;
}

void dilation(ivec2 coordinate) {
	vec4 pixel = imageLoad(lightmapAtlasImage, coordinate);

	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2(-1, -1));
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2( 0, -1));
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2( 1, -1));
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2(-1,  0));
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2( 1,  0));
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2(-1,  1));
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2( 0,  1));
	if (empty(pixel)) pixel = imageLoad(lightmapAtlasImage, coordinate + ivec2( 1,  1));

	imageStore(lightmapAtlasImage, coordinate, pixel);
}

void main() {
	if (gl_GlobalInvocationID.x >= ATLAS_SIZE.x || gl_GlobalInvocationID.y >= ATLAS_SIZE.y) {
		return;
	}

	dilation(ivec2(gl_GlobalInvocationID.xy));
}