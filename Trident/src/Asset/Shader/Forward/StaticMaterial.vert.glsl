#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

out Interstage {
	layout(location = 0) vec3 normal;
	layout(location = 1) vec2 textureCoordinate;
} interstage;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
} camera;

layout(set = 0, binding = 1) buffer readonly TriangleBuffer {
	uint triangleBuffer[];
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
	uint triangleOffset = triangleIndex * 7u;
	uint triangleVertexIndex = vertexIndex - triangleIndex * 3u;

	uint positionOffset = triangleBuffer[triangleOffset + triangleVertexIndex] * 3u;
	uint normalOffset = triangleBuffer[triangleOffset + 3u] * 3u;
	uint textureCoordinateOffset = triangleBuffer[triangleOffset + 4u + triangleVertexIndex] * 2u;

	vec3 vertex = vec3(
		vertexBuffer[positionOffset + 0u],
		vertexBuffer[positionOffset + 1u],
		vertexBuffer[positionOffset + 2u]
	);

	vec3 normal = vec3(
		normalBuffer[normalOffset + 0u],
		normalBuffer[normalOffset + 1u],
		normalBuffer[normalOffset + 2u]
	);

	vec2 textureCoordinate = vec2(
		textureCoordinateBuffer[textureCoordinateOffset + 0u],
		textureCoordinateBuffer[textureCoordinateOffset + 1u]
	);

	gl_Position = camera.projectionView * vec4(vertex, 1.0);

	interstage.normal = normal;
	interstage.textureCoordinate = textureCoordinate;
}