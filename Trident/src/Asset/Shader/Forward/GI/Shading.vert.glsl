#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 a;
layout(location = 2) in vec4 b;
layout(location = 3) in vec4 c;

out Interstage {
	layout(location = 0) vec3 surface;
	layout(location = 1) vec3 a;
	layout(location = 2) vec3 b;
	layout(location = 3) vec3 c;
} interstage;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
} camera;

void main() {
	gl_Position = camera.projectionView * position;

	interstage.surface = position.xyz;
	interstage.a = a.xyz;
	interstage.b = b.xyz;
	interstage.c = c.xyz;
}