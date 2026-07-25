// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

static const uint BILATERAL_FILTER_KERNEL_SIZE = 5;
static const float BILATERAL_FILTER_SPATIAL_GAUSSIAN_RMS = 3.0;
static const float BILATERAL_FILTER_BRIGHTNESS_GAUSSIAN_RMS = 0.1;

RWTexture2D<float4> intermediateLightmapAtlas : register(u0);

float gaussian(float x, float rms) {
	return exp(-(x * x) / (2.0 * rms * rms));
}

// See https://stackoverflow.com/a/6538650/17136841.
[numthreads(16, 16, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 coordinate = globalInvocationIndex.xy;
	uint2 atlasSize;

	intermediateLightmapAtlas.GetDimensions(atlasSize.x, atlasSize.y);

	if (coordinate.x >= atlasSize.x || coordinate.y >= atlasSize.y) {
		return;
	}

	const int kernelRadius = (BILATERAL_FILTER_KERNEL_SIZE - 1) / 2;

	float3 center = intermediateLightmapAtlas[coordinate].rgb;
	float3 avg = float3(0.0, 0.0, 0.0);
	float w = 0.0;
	int sampleIndex = -kernelRadius;

	while (sampleIndex <= kernelRadius) {
		int2 offset = int2(0, sampleIndex);
		float3 value = intermediateLightmapAtlas[coordinate + offset].rgb;
		float spatialGaussian = gaussian(sampleIndex, BILATERAL_FILTER_SPATIAL_GAUSSIAN_RMS);
		float brightnessGaussian = gaussian(distance(value, center), BILATERAL_FILTER_BRIGHTNESS_GAUSSIAN_RMS);

		avg += value * spatialGaussian * brightnessGaussian;
		w += spatialGaussian * brightnessGaussian;

		sampleIndex += 1;
	}

	intermediateLightmapAtlas[coordinate] = float4(avg / w, 1.0);
}