#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec2 textureCoordinate;
layout(location = 4) in mat4x4 worldTransform;
layout(location = 8) in mat3x3 normalTransform;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
	vec3 position;
} camera;

out Interstage {
	layout(location = 0) vec3 surface;
	layout(location = 1) vec2 textureCoordinate;
	layout(location = 2) mat3x3 tbn;
	layout(location = 5) flat uint instanceIndex;
} interstage;

void main() {
	vec4 surface = worldTransform * vec4(position, 1.0);

	vec3 n = normalize(normalTransform * (normal * 2.0 - 1.0));
	vec3 t = normalize(normalTransform * tangent.xyz * 2.0 - 1.0);
	vec3 b = cross(n, t);

	gl_Position = camera.projectionView * surface;

	interstage.surface = surface.xyz;
	interstage.textureCoordinate = textureCoordinate * 2.0 - 1.0;
	interstage.tbn = mat3x3(t, b, n);
	interstage.instanceIndex = uint(gl_InstanceIndex);
}