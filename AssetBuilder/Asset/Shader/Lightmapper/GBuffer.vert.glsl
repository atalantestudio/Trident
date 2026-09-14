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
	layout(location = 12) flat float triangleArea;
} interstage;

struct Vertex {
	vec3 position;
	uint _0;
	vec3 normal;
	uint _1;
};

struct Triangle {
	uint index;
	uint lightmapCoordinateIndexA;
	uint lightmapCoordinateIndexB;
	uint lightmapCoordinateIndexC;
	uint materialIndex;
};

struct Instance {
	mat4x3 transform;
	uint geometryTriangleOffset;
	uint deduplicatedTriangleOffset;
	uint lightmapCoordinateOffset;
};

/* const vec2 LIGHTMAP_COORDINATE_OFFSETS[25] = vec2[](
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
); */

layout(set = 0, binding = 0) buffer readonly VertexBuffer {
	Vertex vertexBuffer[];
};

layout(set = 0, binding = 1) buffer readonly LightmapCoordinateBuffer {
	vec2 lightmapCoordinateBuffer[];
};

layout(set = 0, binding = 2) buffer readonly IndexBuffer {
	uint indexBuffer[];
};

layout(set = 0, binding = 3) buffer readonly TriangleBuffer {
	Triangle triangleBuffer[];
};

layout(set = 0, binding = 4) buffer readonly InstanceBuffer {
	Instance instanceBuffer[];
};

float calculateArea(vec3 a, vec3 b, vec3 c) {
	return length(cross(b - a, c - a)) * 0.5;
}

void main() {
	uint triangleIndex = uint(gl_VertexIndex / 3);

	Instance instance = instanceBuffer[gl_InstanceIndex];
	Triangle triangle = triangleBuffer[instance.deduplicatedTriangleOffset + triangleIndex];

	mat3x3 transform = mat3x3(instance.transform);

	Vertex a = vertexBuffer[indexBuffer[triangleIndex * 3u + 0u]];
	Vertex b = vertexBuffer[indexBuffer[triangleIndex * 3u + 1u]];
	Vertex c = vertexBuffer[indexBuffer[triangleIndex * 3u + 2u]];

	uint index = indexBuffer[gl_VertexIndex];

	Vertex vertex = vertexBuffer[index];
	vec2 coordinate = lightmapCoordinateBuffer[instance.lightmapCoordinateOffset + index];

	// TODO(asset-builder/lightmapper): Get the atlas size.
	// TODO(asset-builder/lightmapper): This fixes thin geometry receiving no light but adds leaks in larger geometry (e.g. floors and walls).
	// coordinate += LIGHTMAP_COORDINATE_OFFSETS[gl_InstanceIndex] * 0.5 / 1024.0;

	coordinate.y = 1.0 - coordinate.y;

	gl_Position = vec4(coordinate * 2.0 - 1.0, 0.0, 1.0);

	interstage.position = instance.transform * vec4(vertex.position, 1.0);
	interstage.triangleIndex = instance.deduplicatedTriangleOffset + triangleIndex + 1u;
	interstage.triangleNormal = vertex.normal * transform;
	interstage.vertexNormal = interstage.triangleNormal;
	interstage.positionA = instance.transform * vec4(a.position, 1.0);
	interstage.positionB = instance.transform * vec4(b.position, 1.0);
	interstage.positionC = instance.transform * vec4(c.position, 1.0);
	interstage.normalA = a.normal * transform;
	interstage.normalB = b.normal * transform;
	interstage.normalC = c.normal * transform;
	interstage.barycentricCoordinate = vec3(0.0, 0.0, 0.0);
	interstage.barycentricCoordinate[gl_VertexIndex % 3] = 1.0;
	interstage.materialIndex = triangle.materialIndex;
	interstage.triangleArea = calculateArea(interstage.positionA, interstage.positionB, interstage.positionC);
}