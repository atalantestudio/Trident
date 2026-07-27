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
	layout(location = 0) flat uint triangleIndex;
	layout(location = 1) vec3 position;
	layout(location = 2) vec3 vertexNormal;
	layout(location = 3) flat vec3 triangleNormal;
} interstage;

void main() {
	uint vertexIndex = index[0];
	uint normalIndex = index[1];

	vec3 position = vec3(vertexBuffer[vertexIndex * 3u + 0u], vertexBuffer[vertexIndex * 3u + 1u], vertexBuffer[vertexIndex * 3u + 2u]);
	vec3 vertexNormal = vec3(normalBuffer[normalIndex * 3u + 0u], normalBuffer[normalIndex * 3u + 1u], normalBuffer[normalIndex * 3u + 2u]);

	vec2 coordinate = lightmapCoordinate;
	coordinate.y = 1.0 - coordinate.y;

	gl_Position = vec4(coordinate * 2.0 - 1.0, 0.0, 1.0);

	interstage.triangleIndex = uint(gl_VertexIndex / 3);
	interstage.position = position;
	interstage.vertexNormal = vertexNormal;
	interstage.triangleNormal = vertexNormal;
}