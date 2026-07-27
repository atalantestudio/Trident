// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct DispatchIndirectCommand {
	uint x;
	uint y;
	uint z;
};

struct TraceRaysArguments {
	uint2 atlasSize;
	uint luxelCount;
	uint triangleCount;
	DispatchIndirectCommand traceRaysCommand;
};

struct Luxel {
	uint triangleIndex;
	uint coordinate;
};

struct Interstage {
	float4 position : SV_Position;
	nointerpolation uint triangleIndex : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 coordinate : VERTEX_OUTPUT_ATTRIBUTE2;
};

static const uint RAYTRACER_WORKGROUP_SIZE = 16;

RWStructuredBuffer<TraceRaysArguments> traceRaysArguments : register(u1);
RWStructuredBuffer<Luxel> luxels : register(u2);

uint ceil(uint value, uint multiplier) {
	return (value + multiplier - 1) & ~(multiplier - 1);
}

void main(Interstage interstage) {
	uint2 coordinate = uint2(interstage.coordinate * traceRaysArguments[0].atlasSize);
	uint luxelIndex;

	InterlockedAdd(traceRaysArguments[0].luxelCount, 1, luxelIndex);

	traceRaysArguments[0].traceRaysCommand.x = ceil(traceRaysArguments[0].luxelCount / RAYTRACER_WORKGROUP_SIZE, RAYTRACER_WORKGROUP_SIZE);

	luxels[luxelIndex].triangleIndex = interstage.triangleIndex;
	luxels[luxelIndex].coordinate = (coordinate.x << 16) | coordinate.y;

	discard;
}