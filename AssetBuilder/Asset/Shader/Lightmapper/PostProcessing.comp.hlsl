// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

static const uint2 ATLAS_SIZE = uint2(2048, 2048);

RWTexture2D<float4> lightmapAtlasImage : register(u0);

bool empty(float4 pixel) {
	return dot(pixel.rgb, pixel.rgb) == 0.0;
}

void dilation(int2 coordinate) {
	float4 pixel = lightmapAtlasImage[coordinate];

	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2(-1, -1)];
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2( 0, -1)];
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2( 1, -1)];
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2(-1,  0)];
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2( 1,  0)];
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2(-1,  1)];
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2( 0,  1)];
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2( 1,  1)];

	lightmapAtlasImage[coordinate] = pixel;
}

[numthreads(32, 32, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	if (globalInvocationIndex.x >= ATLAS_SIZE.x || globalInvocationIndex.y >= ATLAS_SIZE.y) {
		return;
	}

	dilation(globalInvocationIndex.xy);
}