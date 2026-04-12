#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in mat4x4 world;
layout(location = 4) in mat3x3 instanceNormal;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
	vec3 position;
} camera;

layout(set = 0, binding = 1) buffer readonly VertexPositionComponent {
	float vertexPositionComponentBuffer[];
};

layout(set = 0, binding = 2) buffer readonly VertexNormalComponent {
	float vertexNormalComponentBuffer[];
};

layout(set = 0, binding = 3) buffer readonly VertexTextureCoordinateComponent {
	float vertexTextureCoordinateComponentBuffer[];
};

layout(set = 0, binding = 4) buffer readonly TriangleTangentComponent {
	float triangleTangentComponentBuffer[];
};

layout(set = 0, binding = 5) buffer readonly VertexPositionIndex {
	uint vertexPositionIndexBuffer[];
};

layout(set = 0, binding = 6) buffer readonly VertexNormalIndex {
	uint vertexNormalIndexBuffer[];
};

layout(set = 0, binding = 7) buffer readonly VertexTextureCoordinateIndex {
	uint vertexTextureCoordinateIndexBuffer[];
};

out VertexOutput {
	layout(location = 0) vec3 surface;
	layout(location = 1) vec2 textureCoordinate;
	layout(location = 2) mat3x3 TBN;
} vertexOutput;

void main() {
	uint triangleIndex = uint(gl_VertexIndex / 3);
	uint positionIndex = vertexPositionIndexBuffer[gl_VertexIndex] * 3u;
	uint normalIndex = vertexNormalIndexBuffer[gl_VertexIndex] * 3u;
	uint textureCoordinateIndex = vertexTextureCoordinateIndexBuffer[gl_VertexIndex] * 2u;
	uint tangentIndex = triangleIndex * 3u;

	vec3 position = vec3(vertexPositionComponentBuffer[positionIndex + 0u], vertexPositionComponentBuffer[positionIndex + 1u], vertexPositionComponentBuffer[positionIndex + 2u]);
	vec3 normal = vec3(vertexNormalComponentBuffer[normalIndex + 0u], vertexNormalComponentBuffer[normalIndex + 1u], vertexNormalComponentBuffer[normalIndex + 2u]);
	vec2 textureCoordinate = vec2(vertexTextureCoordinateComponentBuffer[textureCoordinateIndex + 0u], vertexTextureCoordinateComponentBuffer[textureCoordinateIndex + 1u]);
	vec3 tangent = vec3(triangleTangentComponentBuffer[tangentIndex + 0u], triangleTangentComponentBuffer[tangentIndex + 1u], triangleTangentComponentBuffer[tangentIndex + 2u]);

	vec4 surface = world * vec4(position, 1.0);

	vec3 N = normalize(instanceNormal * normal);
	vec3 T = normalize(instanceNormal * tangent);
	vec3 B = cross(N, T);

	gl_Position = camera.projectionView * surface;

	vertexOutput.surface = surface.xyz;
	vertexOutput.textureCoordinate = vec2(textureCoordinate.x, 1.0 - textureCoordinate.y);
	vertexOutput.TBN = mat3x3(T, B, N);
}