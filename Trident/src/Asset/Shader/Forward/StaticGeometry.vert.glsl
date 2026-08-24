#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec2 textureCoordinate;
layout(location = 4) in vec2 lightmapCoordinate;
layout(location = 5) in mat4x4 worldTransform;
layout(location = 9) in mat3x3 normalTransform;

out Interstage {
	layout(location = 0) vec3 normal;
	layout(location = 1) vec2 textureCoordinate;
	layout(location = 2) vec2 lightmapCoordinate;
	layout(location = 3) vec4 lightSpacePosition;
} interstage;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
} camera;

layout(set = 0, binding = 2) uniform readonly DirectionalLight {
	mat4x4 projectionView;
} directionalLight;

void main() {
	vec4 vertex = worldTransform * vec4(position, 1.0);

	gl_Position = camera.projectionView * vertex;

	interstage.normal = normal * 2.0 - 1.0;
	interstage.textureCoordinate = textureCoordinate;
	interstage.lightmapCoordinate = lightmapCoordinate;
	interstage.lightSpacePosition = directionalLight.projectionView * vertex;
}