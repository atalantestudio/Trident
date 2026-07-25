// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

RWTexture2D<float4> intermediateLightmapAtlas : register(u0);

void loadLuxel(int2 coordinate, int2 atlasSize, inout float4 color) {
	if (coordinate.x < 0 || coordinate.x >= atlasSize.x) {
		return;
	}

	if (coordinate.y < 0 || coordinate.y >= atlasSize.y) {
		return;
	}

	color = intermediateLightmapAtlas[coordinate];
}

[numthreads(16, 16, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 coordinate = globalInvocationIndex.xy;
	uint2 atlasSize;

	intermediateLightmapAtlas.GetDimensions(atlasSize.x, atlasSize.y);

	if (coordinate.x >= atlasSize.x || coordinate.y >= atlasSize.y) {
		return;
	}

	float4 color = intermediateLightmapAtlas[coordinate];

	if (color.a == 0.0) loadLuxel(coordinate + int2(-1, -1), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + int2( 0, -1), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + int2( 1, -1), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + int2(-1,  0), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + int2( 1,  0), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + int2(-1,  1), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + int2( 0,  1), atlasSize, color);
	if (color.a == 0.0) loadLuxel(coordinate + int2( 1,  1), atlasSize, color);

	intermediateLightmapAtlas[coordinate] = color;
}