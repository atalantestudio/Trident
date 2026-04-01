#version 310 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp image2D;
precision highp uimage2D;

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct Camera {
	mat4x4 projectionView;
	float nearPlane;
	float farPlane;
	uvec2 viewport;
	vec3 position;
};

const uint MATERIAL_INDEX = 1u;
const uint CLUSTER_TRIANGLE_COUNT = 128u;
const uint TRIANGLE_INDEX_BIT_COUNT = 7u;
const uint TRIANGLE_INDEX_BIT_MASK = CLUSTER_TRIANGLE_COUNT - 1u;

layout(set = 0, binding = 0) uniform readonly CameraBuffer {
	Camera camera;
};

layout(set = 0, binding = 1) buffer readonly MaterialOffsetBuffer {
	uint materialOffsetBuffer[];
};

layout(set = 0, binding = 2) buffer readonly MaterialCoordinateBuffer {
	uint materialCoordinateBuffer[];
};

layout(set = 0, binding = 3, rgba8) uniform writeonly image2D outputImage;

layout(set = 0, binding = 4, r32ui) uniform readonly uimage2D visibilityImage;

uint murmurMix(uint hash) {
	uint value = hash;
	value ^= value >> 16;
	value *= 0x85ebca6bu;
	value ^= value >> 13;
	value *= 0xc2b2ae35u;
	value ^= value >> 16;

	return value;
}

vec3 uintToColor(uint value) {
	uint hash = murmurMix(value);

	vec3 color = vec3(
		float((hash >> 0) & 255u),
		float((hash >> 8) & 255u),
		float((hash >> 16) & 255u)
	);

	return color / 255.0;
}

void main() {
	uint globalInvocationIndex = gl_GlobalInvocationID.x;

	if (globalInvocationIndex >= materialOffsetBuffer[MATERIAL_INDEX]) {
		return;
	}

	uint pixelIndex = materialOffsetBuffer[MATERIAL_INDEX - 1u] + globalInvocationIndex;
	uint packedCoordinate = materialCoordinateBuffer[pixelIndex];
	uvec2 coordinate = uvec2(packedCoordinate >> 16, packedCoordinate & 0xffffu);

	uint visibility = imageLoad(visibilityImage, ivec2(coordinate)).r;
	uint clusterIndex = (visibility >> TRIANGLE_INDEX_BIT_COUNT) - 1u;
	uint clusterTriangleIndex = visibility & TRIANGLE_INDEX_BIT_MASK;
	uint triangleIndex = clusterIndex * CLUSTER_TRIANGLE_COUNT + clusterTriangleIndex;

	vec3 color = uintToColor(triangleIndex + 1u);

	imageStore(outputImage, ivec2(coordinate), vec4(color, 1.0));
}