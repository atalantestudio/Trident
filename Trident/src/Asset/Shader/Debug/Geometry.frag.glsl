#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

in Interstage {
	layout(location = 0) vec3 surface;
	layout(location = 1) vec3 normal;
} interstage;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
	vec3 position;
} camera;

layout(location = 0) out vec4 fragment;

void main() {
	vec3 surfaceToCamera = normalize(camera.position - interstage.surface);
	float angle = dot(interstage.normal, surfaceToCamera);

	fragment = vec4(vec3(angle), 1.0);
}