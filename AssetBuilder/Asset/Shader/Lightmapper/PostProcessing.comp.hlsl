// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

static const uint BILATERAL_FILTER_1D_SAMPLE_COUNT = 11;
static const float BILATERAL_FILTER_SPATIAL_GAUSSIAN_RMS = 3.0;
static const float BILATERAL_FILTER_BRIGHTNESS_GAUSSIAN_RMS = 0.1;

RWTexture2D<float4> lightmapAtlasImage : register(u0);

bool empty(float4 pixel) {
	return dot(pixel.rgb, pixel.rgb) == 0.0;
}

float gaussian(float x, float rms) {
	return exp(-(x * x) / (2.0 * rms * rms));
}

void denoise(int2 coordinate) {
	const int kernelRadius = (int(BILATERAL_FILTER_1D_SAMPLE_COUNT) - 1) / 2;

	float3 center = lightmapAtlasImage[coordinate].rgb;
	float3 avg = float3(0.0, 0.0, 0.0);
	float w = 0.0;
	int sampleIndex = -kernelRadius;

	// Apply horizontal blur.
	while (sampleIndex <= kernelRadius) {
		int2 offset = int2(sampleIndex, 0);
		float3 value = lightmapAtlasImage[coordinate + offset].rgb;

		float spatialGaussian = gaussian(sampleIndex, BILATERAL_FILTER_SPATIAL_GAUSSIAN_RMS);
		float brightnessGaussian = gaussian(length(value - center), BILATERAL_FILTER_BRIGHTNESS_GAUSSIAN_RMS);

		avg += value * spatialGaussian * brightnessGaussian;
		w += spatialGaussian * brightnessGaussian;

		sampleIndex += 1;
	}

	lightmapAtlasImage[coordinate] = float4(avg / w, 1.0);

	GroupMemoryBarrierWithGroupSync();

	avg = float3(0.0, 0.0, 0.0);
	w = 0.0;
	sampleIndex = -kernelRadius;

	// Apply vertical blur.
	while (sampleIndex <= kernelRadius) {
		int2 offset = int2(0, sampleIndex);
		float3 value = lightmapAtlasImage[coordinate + offset].rgb;

		float spatialGaussian = gaussian(sampleIndex, BILATERAL_FILTER_SPATIAL_GAUSSIAN_RMS);
		float brightnessGaussian = gaussian(length(value - center), BILATERAL_FILTER_BRIGHTNESS_GAUSSIAN_RMS);

		avg += value * spatialGaussian * brightnessGaussian;
		w += spatialGaussian * brightnessGaussian;

		sampleIndex += 1;
	}

	lightmapAtlasImage[coordinate] = float4(avg / w, 1.0);
}

void loadLightmapPixel(int2 coordinate, int2 atlasSize, inout float4 pixel) {
	if (coordinate.x < 0 || coordinate.x >= atlasSize.x) {
		return;
	}

	if (coordinate.y < 0 || coordinate.y >= atlasSize.y) {
		return;
	}

	pixel = lightmapAtlasImage[coordinate];
}

void dilate(int2 coordinate, int2 atlasSize) {
	float4 pixel = lightmapAtlasImage[coordinate];

	// Horizontal
	if (empty(pixel)) loadLightmapPixel(coordinate + int2(-1,  0), atlasSize, pixel);
	if (empty(pixel)) loadLightmapPixel(coordinate + int2( 1,  0), atlasSize, pixel);

	// Vertical
	if (empty(pixel)) loadLightmapPixel(coordinate + int2( 0,  1), atlasSize, pixel);
	if (empty(pixel)) loadLightmapPixel(coordinate + int2( 0, -1), atlasSize, pixel);

	// Corners
	if (empty(pixel)) loadLightmapPixel(coordinate + int2(-1,  1), atlasSize, pixel);
	if (empty(pixel)) loadLightmapPixel(coordinate + int2( 1,  1), atlasSize, pixel);
	if (empty(pixel)) loadLightmapPixel(coordinate + int2( 1, -1), atlasSize, pixel);
	if (empty(pixel)) loadLightmapPixel(coordinate + int2(-1, -1), atlasSize, pixel);

	lightmapAtlasImage[coordinate] = pixel;
}

[numthreads(32, 32, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 coordinate = globalInvocationIndex.xy;
	uint2 atlasSize;

	lightmapAtlasImage.GetDimensions(atlasSize.x, atlasSize.y);

	if (coordinate.x >= atlasSize.x || coordinate.y >= atlasSize.y) {
		return;
	}

	denoise(coordinate);

	GroupMemoryBarrierWithGroupSync();

	dilate(coordinate, atlasSize);
}