#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct Camera {
	mat4x4 projectionView;
	float nearPlane;
	float farPlane;
	uvec2 viewport;
	vec3 position;
};

const uint MATERIAL_INDEX = 1;

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

void main() {
	uint globalInvocationIndex = gl_GlobalInvocationID.x;
	uint pixelIndex = materialOffsetBuffer[MATERIAL_INDEX - 1] + globalInvocationIndex;
	uint packedCoordinate = materialCoordinateBuffer[pixelIndex];
	uvec2 coordinate = uvec2(packedCoordinate >> 16, packedCoordinate & 0xffffu);

	// TODO
	imageStore(outputImage, ivec2(coordinate), vec4(1.0, 0.2, 0.0, 1.0));
}