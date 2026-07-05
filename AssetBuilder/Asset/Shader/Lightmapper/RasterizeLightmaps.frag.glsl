#version 440

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct DispatchIndirectCommand {
	uint x;
	uint y;
	uint z;
};

struct LightmapPixel {
	uint triangleIndex;
	uint coordinate;
};

in Interstage {
	layout(location = 0) flat uint triangleIndex;
	layout(location = 1) vec2 coordinate;
} interstage;

const uint RAYTRACER_WORKGROUP_SIZE = 16;
const uvec2 ATLAS_SIZE = uvec2(2048, 2048);

layout(set = 0, binding = 1) buffer TraceRaysArgumentBuffer {
	uint lightmapPixelCount;
	uint triangleCount;
	DispatchIndirectCommand traceRaysCommand;
};

layout(set = 0, binding = 2) buffer writeonly LightmapPixelBuffer {
	LightmapPixel lightmapPixels[];
};

layout(location = 0) out vec4 fragment;

void main() {
	uvec2 coordinate = uvec2(interstage.coordinate * ATLAS_SIZE);
	uint lightmapPixelIndex = atomicAdd(lightmapPixelCount, 1);

	traceRaysCommand.x = lightmapPixelCount / RAYTRACER_WORKGROUP_SIZE;

	lightmapPixels[lightmapPixelIndex].triangleIndex = interstage.triangleIndex;
	lightmapPixels[lightmapPixelIndex].coordinate = (coordinate.x << 16) | coordinate.y;

	discard;
}