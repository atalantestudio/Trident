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

layout(set = 0, binding = 0) uniform readonly CameraBuffer {
	Camera camera;
};

layout(set = 0, binding = 1) buffer readonly MaterialCoordinateBuffer {
	uint materialCoordinateBuffer[];
};

layout(set = 0, binding = 2, rgba8) uniform writeonly image2D outputImage;

layout(set = 0, binding = 3, r32ui) uniform readonly uimage2D visibilityImage;

void main() {
	uint index = gl_GlobalInvocationID.x;
	uint packedCoordinate = materialCoordinateBuffer[index];
	uvec2 coordinate = uvec2(packedCoordinate >> 16, packedCoordinate & uint(0xffff));

	// imageStore(outputImage, ivec2(coordinate), vec4(1.0, 0.2, 0.0, 1.0));
}