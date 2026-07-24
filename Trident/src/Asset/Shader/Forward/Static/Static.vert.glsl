#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Vertex {
	uint vertexIndex;
	uint textureCoordinateIndex;
	vec2 lightmapCoordinate;
};

struct Triangle {
	Vertex vertices[3];
	uint normalIndex;
};

out Interstage {
	layout(location = 0) vec3 normal;
	layout(location = 1) vec2 textureCoordinate;
	layout(location = 2) vec2 lightmapCoordinate;
} interstage;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
} camera;

layout(set = 0, binding = 1) buffer readonly TriangleBuffer {
	Triangle triangleBuffer[];
};

layout(set = 0, binding = 2) buffer readonly VertexBuffer {
	float vertexBuffer[];
};

layout(set = 0, binding = 3) buffer readonly NormalBuffer {
	float normalBuffer[];
};

layout(set = 0, binding = 4) buffer readonly TextureCoordinateBuffer {
	float textureCoordinateBuffer[];
};

void main() {
	uint vertexIndex = uint(gl_VertexIndex);
	uint triangleIndex = vertexIndex / 3u;
	uint triangleVertexIndex = vertexIndex - triangleIndex * 3u;

	Triangle triangle = triangleBuffer[triangleIndex];
	Vertex triangleVertex = triangle.vertices[triangleVertexIndex];

	uint vertexOffset = triangleVertex.vertexIndex * 3u;
	uint normalOffset = triangle.normalIndex * 3u;
	uint textureCoordinateOffset = triangleVertex.textureCoordinateIndex * 2u;

	vec3 vertex = vec3(vertexBuffer[vertexOffset + 0u], vertexBuffer[vertexOffset + 1u], vertexBuffer[vertexOffset + 2u]);
	vec3 normal = vec3(normalBuffer[normalOffset + 0u], normalBuffer[normalOffset + 1u], normalBuffer[normalOffset + 2u]);
	vec2 textureCoordinate = vec2(textureCoordinateBuffer[textureCoordinateOffset + 0u], textureCoordinateBuffer[textureCoordinateOffset + 1u]);

	gl_Position = camera.projectionView * vec4(vertex, 1.0);

	interstage.normal = normal;
	interstage.textureCoordinate = textureCoordinate;
	interstage.lightmapCoordinate = triangleVertex.lightmapCoordinate;
}