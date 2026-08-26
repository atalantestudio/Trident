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
	layout(location = 0) vec3 surface;
	layout(location = 1) mat3x3 tbn;
	layout(location = 4) vec2 textureCoordinate;
	layout(location = 5) vec4 lightSpacePosition;
	layout(location = 6) vec2 lightmapCoordinate;
} interstage;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
} camera;

layout(set = 0, binding = 1) uniform readonly DirectionalLight {
	mat4x4 projectionView;
} directionalLight;

void main() {
	vec4 surface = worldTransform * vec4(position, 1.0);
	vec3 n = normalize(normalTransform * (normal * 2.0 - 1.0));
	vec3 t = normalize(normalTransform * tangent.xyz * 2.0 - 1.0);
	vec3 b = cross(n, t);

	gl_Position = camera.projectionView * surface;

	interstage.surface = surface.xyz;
	interstage.tbn = mat3x3(t, b, n);
	interstage.textureCoordinate = textureCoordinate;
	interstage.lightSpacePosition = directionalLight.projectionView * surface;
	interstage.lightmapCoordinate = lightmapCoordinate;
}