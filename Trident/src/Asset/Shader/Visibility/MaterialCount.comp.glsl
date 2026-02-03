#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

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

layout(set = 0, binding = 1, r8) uniform readonly image2D materialIndexImage;

layout(set = 0, binding = 2) buffer writeonly MaterialCount {
	uint materialCountBuffer[];
};

void main() {
	uvec2 position = gl_GlobalInvocationID.xy;

	if (position.x >= camera.viewport.x || position.y >= camera.viewport.y) {
		return;
	}

	uint materialIndex = uint(imageLoad(materialIndexImage, ivec2(position)).r);

	atomicAdd(materialCountBuffer[materialIndex], 1);
}