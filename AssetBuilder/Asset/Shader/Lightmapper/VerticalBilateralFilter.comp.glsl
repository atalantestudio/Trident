#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

const uint BILATERAL_FILTER_KERNEL_SIZE = 5;
const float BILATERAL_FILTER_SPATIAL_GAUSSIAN_RMS = 3.0;
const float BILATERAL_FILTER_BRIGHTNESS_GAUSSIAN_RMS = 0.1;

layout(set = 0, binding = 0, rgba8) uniform image2D intermediateLightmapAtlas;

float gaussian(float x, float rms) {
	return exp(-(x * x) / (2.0 * rms * rms));
}

// See https://stackoverflow.com/a/6538650/17136841.
void main() {
	ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
	ivec2 atlasSize = imageSize(intermediateLightmapAtlas);

	if (coordinate.x >= atlasSize.x || coordinate.y >= atlasSize.y) {
		return;
	}

	const int kernelRadius = (int(BILATERAL_FILTER_KERNEL_SIZE) - 1) / 2;

	vec3 center = imageLoad(intermediateLightmapAtlas, coordinate).rgb;
	vec3 avg = vec3(0.0, 0.0, 0.0);
	float w = 0.0;
	int sampleIndex = -kernelRadius;

	while (sampleIndex <= kernelRadius) {
		ivec2 offset = ivec2(0, sampleIndex);
		vec3 value = imageLoad(intermediateLightmapAtlas, coordinate + offset).rgb;

		float spatialGaussian = gaussian(sampleIndex, BILATERAL_FILTER_SPATIAL_GAUSSIAN_RMS);
		float brightnessGaussian = gaussian(distance(value, center), BILATERAL_FILTER_BRIGHTNESS_GAUSSIAN_RMS);

		avg += value * spatialGaussian * brightnessGaussian;
		w += spatialGaussian * brightnessGaussian;

		sampleIndex += 1;
	}

	imageStore(intermediateLightmapAtlas, coordinate, vec4(avg / w, 1.0));
}