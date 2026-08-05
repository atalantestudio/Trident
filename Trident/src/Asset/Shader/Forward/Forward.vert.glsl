#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in uvec3 vertex;
layout(location = 1) in mat4x4 world;
layout(location = 5) in mat3x3 instanceNormal;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
	vec3 position;
} camera;

layout(set = 0, binding = 1) buffer readonly PositionBuffer {
	float positionBuffer[];
};

layout(set = 0, binding = 2) buffer readonly NormalBuffer {
	float normalBuffer[];
};

layout(set = 0, binding = 3) buffer readonly TextureCoordinateBuffer {
	float textureCoordinateBuffer[];
};

out Interstage {
	layout(location = 0) vec3 surface;
	layout(location = 1) vec2 textureCoordinate;
	layout(location = 2) mat3x3 tbn;
} interstage;

void main() {
	uint positionOffset = vertex[0] * 3u;
	uint normalOffset = vertex[1] * 3u;
	uint textureCoordinateOffset = vertex[2] * 2u;

	vec3 position = vec3(positionBuffer[positionOffset + 0u], positionBuffer[positionOffset + 1u], positionBuffer[positionOffset + 2u]);
	vec3 normal = vec3(normalBuffer[normalOffset + 0u], normalBuffer[normalOffset + 1u], normalBuffer[normalOffset + 2u]);
	vec2 textureCoordinate = vec2(textureCoordinateBuffer[textureCoordinateOffset + 0u], textureCoordinateBuffer[textureCoordinateOffset + 1u]);

	// TODO(forward)
	vec3 tangent = normal;

	vec4 surface = world * vec4(position, 1.0);

	vec3 n = normalize(instanceNormal * normal);
	vec3 t = normalize(instanceNormal * tangent);
	vec3 b = cross(n, t);

	gl_Position = camera.projectionView * surface;

	interstage.surface = surface.xyz;
	interstage.textureCoordinate = textureCoordinate;
	interstage.textureCoordinate.y = 1.0 - textureCoordinate.y;
	interstage.tbn = mat3x3(t, b, n);
}