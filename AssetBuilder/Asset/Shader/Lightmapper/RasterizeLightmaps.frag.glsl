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
} interstage;

layout(set = 0, binding = 0) buffer TraceRaysArgumentBuffer {
	uint lightmapPixelCount;
	uint triangleCount;
	DispatchIndirectCommand traceRaysCommand;
};

layout(set = 0, binding = 1) buffer writeonly LightmapPixelBuffer {
	LightmapPixel lightmapPixels[];
};

layout(location = 0) out vec4 fragment;

void main() {
	uvec2 coordinate = uvec2(gl_FragCoord.xy);
	uint lightmapPixelIndex = atomicAdd(lightmapPixelCount, 1);

	traceRaysCommand.x = lightmapPixelCount / 16;

	lightmapPixels[lightmapPixelIndex].triangleIndex = interstage.triangleIndex;
	lightmapPixels[lightmapPixelIndex].coordinate = (coordinate.x << 16) | coordinate.y;

	discard;
}