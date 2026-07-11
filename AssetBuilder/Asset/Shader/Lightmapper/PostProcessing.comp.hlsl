// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

RWTexture2D<float4> lightmapAtlasImage : register(u0);

bool empty(float4 pixel) {
	return dot(pixel.rgb, pixel.rgb) == 0.0;
}

void dilation(int2 coordinate) {
	float4 pixel = lightmapAtlasImage[coordinate];

	// Horizontal
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2(-1,  0)];
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2( 1,  0)];

	// Vertical
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2( 0,  1)];
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2( 0, -1)];

	// Corners
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2(-1,  1)];
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2( 1,  1)];
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2( 1, -1)];
	if (empty(pixel)) pixel = lightmapAtlasImage[coordinate + int2(-1, -1)];

	lightmapAtlasImage[coordinate] = pixel;
}

[numthreads(32, 32, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 atlasSize;

	lightmapAtlasImage.GetDimensions(atlasSize.x, atlasSize.y);

	if (globalInvocationIndex.x >= atlasSize.x || globalInvocationIndex.y >= atlasSize.y) {
		return;
	}

	dilation(globalInvocationIndex.xy);
}