#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
	vec3 position;
} camera;

out Interstage {
	layout(location = 0) vec3 surface;
	layout(location = 1) vec3 normal;
} interstage;

void main() {
	gl_Position = camera.projectionView * vec4(position, 1.0);

	interstage.surface = position;
	interstage.normal = normal * 2.0 - 1.0;
}