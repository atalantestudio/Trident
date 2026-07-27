#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in vec2 lightmapCoordinate;
layout(location = 1) in uvec4 index;

layout(set = 0, binding = 0) buffer readonly VertexBuffer {
	float vertexBuffer[];
};

layout(set = 0, binding = 1) buffer readonly NormalBuffer {
	float normalBuffer[];
};

out Interstage {
	layout(location = 0) vec3 position;
	layout(location = 1) flat uint triangleIndex;
	layout(location = 2) flat vec3 triangleNormal;
	layout(location = 3) vec3 vertexNormal;
} interstage;

void main() {
	uint vertexOffset = index[0] * 3u;
	uint normalOffset = index[1] * 3u;

	vec3 position = vec3(vertexBuffer[vertexOffset + 0u], vertexBuffer[vertexOffset + 1u], vertexBuffer[vertexOffset + 2u]);
	vec3 vertexNormal = vec3(normalBuffer[normalOffset + 0u], normalBuffer[normalOffset + 1u], normalBuffer[normalOffset + 2u]);

	vec2 coordinate = lightmapCoordinate;
	coordinate.y = 1.0 - coordinate.y;

	gl_Position = vec4(coordinate * 2.0 - 1.0, 0.0, 1.0);

	interstage.position = position;
	interstage.triangleIndex = uint(gl_VertexIndex / 3 + 1);
	interstage.triangleNormal = vertexNormal;
	interstage.vertexNormal = vertexNormal;
}