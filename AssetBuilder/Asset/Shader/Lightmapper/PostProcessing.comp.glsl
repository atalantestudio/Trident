#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

const uint BILATERAL_FILTER_1D_SAMPLE_COUNT = 11;
const float BILATERAL_FILTER_SPATIAL_GAUSSIAN_RMS = 3.0;
const float BILATERAL_FILTER_BRIGHTNESS_GAUSSIAN_RMS = 0.1;

layout(set = 0, binding = 0, rgba8) uniform image2D lightmapAtlasImage;

bool empty(vec4 pixel) {
	return any(equal(pixel.rgb, vec3(0.0, 0.0, 0.0)));
}

float gaussian(float x, float rms) {
	return exp(-(x * x) / (2.0 * rms * rms));
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

void denoise(ivec2 coordinate) {
	const int kernelRadius = (int(BILATERAL_FILTER_1D_SAMPLE_COUNT) - 1) / 2;

	vec3 center = imageLoad(lightmapAtlasImage, coordinate).rgb;
	vec3 avg = vec3(0.0, 0.0, 0.0);
	float w = 0.0;
	int sampleIndex = -kernelRadius;

	// Apply horizontal blur.
	while (sampleIndex <= kernelRadius) {
		ivec2 offset = ivec2(sampleIndex, 0);
		vec3 value = imageLoad(lightmapAtlasImage, coordinate + offset).rgb;

		float spatialGaussian = gaussian(sampleIndex, BILATERAL_FILTER_SPATIAL_GAUSSIAN_RMS);
		float brightnessGaussian = gaussian(length(value - center), BILATERAL_FILTER_BRIGHTNESS_GAUSSIAN_RMS);

		avg += value * spatialGaussian * brightnessGaussian;
		w += spatialGaussian * brightnessGaussian;

		sampleIndex += 1;
	}

	imageStore(lightmapAtlasImage, coordinate, vec4(avg / w, 1.0));

	memoryBarrierImage();

	avg = vec3(0.0, 0.0, 0.0);
	w = 0.0;
	sampleIndex = -kernelRadius;

	// Apply vertical blur.
	while (sampleIndex <= kernelRadius) {
		ivec2 offset = ivec2(0, sampleIndex);
		vec3 value = imageLoad(lightmapAtlasImage, coordinate + offset).rgb;

		float spatialGaussian = gaussian(sampleIndex, BILATERAL_FILTER_SPATIAL_GAUSSIAN_RMS);
		float brightnessGaussian = gaussian(length(value - center), BILATERAL_FILTER_BRIGHTNESS_GAUSSIAN_RMS);

		avg += value * spatialGaussian * brightnessGaussian;
		w += spatialGaussian * brightnessGaussian;

		sampleIndex += 1;
	}

	imageStore(lightmapAtlasImage, coordinate, vec4(avg / w, 1.0));
}

void main() {
	ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
	ivec2 atlasSize = imageSize(lightmapAtlasImage);

	if (coordinate.x >= atlasSize.x || coordinate.y >= atlasSize.y) {
		return;
	}

	dilate(coordinate);

	memoryBarrierImage();

	denoise(coordinate);
}