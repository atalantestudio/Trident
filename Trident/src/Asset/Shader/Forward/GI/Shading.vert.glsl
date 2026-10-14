#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in vec3 position;

out Interstage {
	layout(location = 0) vec3 surface;
} interstage;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
} camera;

void main() {
	vec4 surface = vec4(position, 1.0);

	gl_Position = camera.projectionView * vec4(position, 1.0);

	interstage.surface = position;
}