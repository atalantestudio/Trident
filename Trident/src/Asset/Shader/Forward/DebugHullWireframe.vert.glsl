#version 310 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in mat4x4 world;
layout(location = 4) in mat3x3 normal;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
} camera;

layout(set = 0, binding = 1) buffer readonly VertexPositionComponent {
	float vertexPositionComponentBuffer[];
};

void main() {
	uint positionComponentOffset = uint(gl_VertexIndex * 3);

	vec3 position = vec3(
		vertexPositionComponentBuffer[positionComponentOffset + 0u],
		vertexPositionComponentBuffer[positionComponentOffset + 1u],
		vertexPositionComponentBuffer[positionComponentOffset + 2u]
	);

	gl_Position = camera.projectionView * world * vec4(position, 1.0);
}