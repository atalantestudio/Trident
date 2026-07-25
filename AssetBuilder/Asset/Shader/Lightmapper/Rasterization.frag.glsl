#version 440

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct DispatchIndirectCommand {
	uint x;
	uint y;
	uint z;
};

struct Luxel {
	uint triangleIndex;
	uint coordinate;
};

in Interstage {
	layout(location = 0) flat uint triangleIndex;
	layout(location = 1) vec2 coordinate;
} interstage;

const uint RAYTRACER_WORKGROUP_SIZE = 16;

layout(set = 0, binding = 0) buffer TraceRaysArgumentBuffer {
	uvec2 atlasSize;
	uint luxelCount;
	uint triangleCount;
	DispatchIndirectCommand traceRaysCommand;
};

layout(set = 0, binding = 1) buffer writeonly LuxelBuffer {
	Luxel luxels[];
};

uint ceil(uint value, uint multiplier) {
	return (value + multiplier - 1) & ~(multiplier - 1);
}

void main() {
	uvec2 coordinate = uvec2(interstage.coordinate * atlasSize);
	uint luxelIndex = atomicAdd(luxelCount, 1);

	traceRaysCommand.x = ceil(luxelCount / RAYTRACER_WORKGROUP_SIZE, RAYTRACER_WORKGROUP_SIZE);

	luxels[luxelIndex].triangleIndex = interstage.triangleIndex;
	luxels[luxelIndex].coordinate = (coordinate.x << 16) | coordinate.y;

	discard;
}