#version 310 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in mat4x4 transform;
layout(location = 4) in vec3 position;

layout(set = 0, binding = 0) uniform Camera {
	mat4x4 projectionView;
} camera;

void main() {
	gl_Position = camera.projectionView * transform * vec4(position, 1.0);
}