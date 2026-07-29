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

const vec2 LIGHTMAP_COORDINATE_OFFSETS[25] = vec2[](
	vec2(-2.0, -2.0),
	vec2( 2.0, -2.0),
	vec2(-2.0,  2.0),
	vec2( 2.0,  2.0),
	vec2(-1.0, -2.0),
	vec2( 1.0, -2.0),
	vec2(-2.0, -1.0),
	vec2( 2.0, -1.0),
	vec2(-2.0,  1.0),
	vec2( 2.0,  1.0),
	vec2(-1.0,  2.0),
	vec2( 1.0,  2.0),
	vec2(-2.0,  0.0),
	vec2( 2.0,  0.0),
	vec2( 0.0, -2.0),
	vec2( 0.0,  2.0),
	vec2(-1.0, -1.0),
	vec2( 1.0, -1.0),
	vec2(-1.0,  0.0),
	vec2( 1.0,  0.0),
	vec2(-1.0,  1.0),
	vec2( 1.0,  1.0),
	vec2( 0.0, -1.0),
	vec2( 0.0,  1.0),
	vec2( 0.0,  0.0)
);

void main() {
	uint vertexOffset = index[0] * 3u;
	uint normalOffset = index[1] * 3u;

	vec3 position = vec3(vertexBuffer[vertexOffset + 0u], vertexBuffer[vertexOffset + 1u], vertexBuffer[vertexOffset + 2u]);
	vec3 vertexNormal = vec3(normalBuffer[normalOffset + 0u], normalBuffer[normalOffset + 1u], normalBuffer[normalOffset + 2u]);

	vec2 coordinate = lightmapCoordinate;

	// TODO(asset-builder/lightmapper): Get the atlas size.
	// TODO(asset-builder/lightmapper): This fixes thin geometry receiving no light but adds leaks in larger geometry (e.g. floors and walls).
	coordinate += LIGHTMAP_COORDINATE_OFFSETS[gl_InstanceIndex] * 0.5 / 1024.0;

	coordinate.y = 1.0 - coordinate.y;

	gl_Position = vec4(coordinate * 2.0 - 1.0, 1.0, 1.0);

	interstage.position = position;
	interstage.triangleIndex = uint(gl_VertexIndex / 3 + 1);
	interstage.triangleNormal = vertexNormal;
	interstage.vertexNormal = vertexNormal;
}