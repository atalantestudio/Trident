#version 310 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in vec3 position;
layout(location = 1) in mat4x4 worldTransform;

layout(set = 0, binding = 0) uniform readonly DirectionalLight {
	mat4x4 projectionView;
} directionalLight;

void main() {
	gl_Position = directionalLight.projectionView * worldTransform * vec4(position, 1.0);
}