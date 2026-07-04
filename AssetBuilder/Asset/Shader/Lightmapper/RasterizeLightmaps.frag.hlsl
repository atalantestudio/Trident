// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct DispatchIndirectCommand {
	uint x;
	uint y;
	uint z;
};

struct TraceRaysArguments {
	uint lightmapPixelCount;
	uint triangleCount;
	DispatchIndirectCommand traceRaysCommand;
};

struct LightmapPixel {
	uint triangleIndex;
	uint coordinate;
};

struct Interstage {
	float4 position : SV_Position;
	nointerpolation uint triangleIndex : VERTEX_OUTPUT_ATTRIBUTE1;
};

RWStructuredBuffer<TraceRaysArguments> traceRaysArguments : register(u1);
RWStructuredBuffer<LightmapPixel> lightmapPixels : register(u2);

void main(Interstage interstage) {
	uint2 coordinate = uint2(interstage.position.xy);
	uint lightmapPixelIndex;

	InterlockedAdd(traceRaysArguments[0].lightmapPixelCount, 1, lightmapPixelIndex);

	traceRaysArguments[0].traceRaysCommand.x = traceRaysArguments[0].lightmapPixelCount / 16;

	lightmapPixels[lightmapPixelIndex].triangleIndex = interstage.triangleIndex;
	lightmapPixels[lightmapPixelIndex].coordinate = (coordinate.x << 16) | coordinate.y;

	discard;
}