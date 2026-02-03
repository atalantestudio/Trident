#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in mat4x4 transform;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
} camera;

layout(set = 0, binding = 1) buffer readonly VertexPositionComponent {
	float vertexPositionComponentBuffer[];
};

layout(set = 0, binding = 2) buffer readonly VertexPositionIndex {
	uint vertexPositionIndexBuffer[];
};

layout(set = 0, binding = 3) buffer readonly VertexNormalComponent {
	float vertexNormalComponentBuffer[];
};

layout(set = 0, binding = 4) buffer readonly VertexNormalIndex {
	uint vertexNormalIndexBuffer[];
};

layout(set = 0, binding = 5) buffer readonly VertexTextureCoordinateComponent {
	float vertexTextureCoordinateComponentBuffer[];
};

layout(set = 0, binding = 6) buffer readonly VertexTextureCoordinateIndex {
	uint vertexTextureCoordinateIndexBuffer[];
};

out VertexOutput {
	layout(location = 0) vec3 normal;
	layout(location = 1) vec2 textureCoordinate;
} vertexOutput;

void main() {
	uint positionIndex = vertexPositionIndexBuffer[gl_VertexIndex];
	uint normalIndex = vertexNormalIndexBuffer[gl_VertexIndex];
	uint textureCoordinateIndex = vertexTextureCoordinateIndexBuffer[gl_VertexIndex];

	vec3 position = vec3(
		vertexPositionComponentBuffer[positionIndex * 3u + 0u],
		vertexPositionComponentBuffer[positionIndex * 3u + 1u],
		vertexPositionComponentBuffer[positionIndex * 3u + 2u]
	);

	vec3 normal = vec3(
		vertexNormalComponentBuffer[normalIndex * 3u + 0u],
		vertexNormalComponentBuffer[normalIndex * 3u + 1u],
		vertexNormalComponentBuffer[normalIndex * 3u + 2u]
	);

	vec2 textureCoordinate = vec2(
		vertexTextureCoordinateComponentBuffer[textureCoordinateIndex * 2u + 0u],
		vertexTextureCoordinateComponentBuffer[textureCoordinateIndex * 2u + 1u]
	);

	gl_Position = camera.projectionView * transform * vec4(position, 1.0);

	vertexOutput.normal = normal;
	vertexOutput.textureCoordinate = textureCoordinate;
}