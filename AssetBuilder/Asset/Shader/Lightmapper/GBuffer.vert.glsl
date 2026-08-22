#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

out Interstage {
	layout(location = 0) vec3 position;
	layout(location = 1) flat uint triangleIndex;
	layout(location = 2) flat vec3 triangleNormal;
	layout(location = 3) vec3 vertexNormal;
	layout(location = 4) flat vec3 positionA;
	layout(location = 5) flat vec3 positionB;
	layout(location = 6) flat vec3 positionC;
	layout(location = 7) flat vec3 normalA;
	layout(location = 8) flat vec3 normalB;
	layout(location = 9) flat vec3 normalC;
	layout(location = 10) vec3 barycentricCoordinate;
	layout(location = 11) flat uint materialIndex;
} interstage;

struct Vertex {
	vec3 position;
	vec3 normal;
	vec4 tangent;
	vec2 lightmapCoordinate;
};

layout(set = 0, binding = 0) buffer readonly VertexBuffer {
	Vertex vertexBuffer[];
};

layout(set = 0, binding = 1) buffer readonly IndexBuffer {
	uint indexBuffer[];
};

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
	Vertex a = vertexBuffer[indexBuffer[gl_VertexIndex % 3 + 0]];
	Vertex b = vertexBuffer[indexBuffer[gl_VertexIndex % 3 + 1]];
	Vertex c = vertexBuffer[indexBuffer[gl_VertexIndex % 3 + 2]];
	Vertex vertex = vertexBuffer[indexBuffer[gl_VertexIndex]];

	vec2 coordinate = vertex.lightmapCoordinate;

	// TODO(asset-builder/lightmapper): Get the atlas size.
	// TODO(asset-builder/lightmapper): This fixes thin geometry receiving no light but adds leaks in larger geometry (e.g. floors and walls).
	// coordinate += LIGHTMAP_COORDINATE_OFFSETS[gl_InstanceIndex] * 0.5 / 1024.0;

	coordinate.y = 1.0 - coordinate.y;

	gl_Position = vec4(coordinate * 2.0 - 1.0, 1.0, 1.0);

	interstage.position = vertex.position;
	interstage.triangleIndex = uint(gl_VertexIndex / 3 + 1);
	interstage.triangleNormal = vertex.normal;
	interstage.vertexNormal = vertex.normal;
	interstage.positionA = a.position;
	interstage.positionB = b.position;
	interstage.positionC = c.position;
	interstage.normalA = a.normal;
	interstage.normalB = b.normal;
	interstage.normalC = c.normal;
	interstage.barycentricCoordinate = vec3(0.0);
	interstage.barycentricCoordinate[gl_VertexIndex % 3] = 1.0;
	interstage.materialIndex = uint(vertex.tangent.w);
}