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

out VertexOutput {
	layout(location = 0) vec3 surface;
	layout(location = 1) vec2 textureCoordinate;
	layout(location = 2) mat3x3 TBN;
} vertexOutput;

void main() {
	uint positionComponentOffset = vertex[0] * 3u;
	uint normalComponentOffset = vertex[1] * 3u;
	uint textureCoordinateComponentOffset = vertex[2] * 2u;
	uint tangentComponentOffset = uint(gl_VertexIndex / 3) * 3u;

	vec3 position = vec3(
		vertexPositionComponentBuffer[positionComponentOffset + 0u],
		vertexPositionComponentBuffer[positionComponentOffset + 1u],
		vertexPositionComponentBuffer[positionComponentOffset + 2u]
	);

	vec3 normal = vec3(
		vertexNormalComponentBuffer[normalComponentOffset + 0u],
		vertexNormalComponentBuffer[normalComponentOffset + 1u],
		vertexNormalComponentBuffer[normalComponentOffset + 2u]
	);

	vec2 textureCoordinate = vec2(
		vertexTextureCoordinateComponentBuffer[textureCoordinateComponentOffset + 0u],
		vertexTextureCoordinateComponentBuffer[textureCoordinateComponentOffset + 1u]
	);

	vec3 tangent = vec3(
		triangleTangentComponentBuffer[tangentComponentOffset + 0u],
		triangleTangentComponentBuffer[tangentComponentOffset + 1u],
		triangleTangentComponentBuffer[tangentComponentOffset + 2u]
	);

	vec4 surface = world * vec4(position, 1.0);

	vec3 N = normalize(instanceNormal * normal);
	vec3 T = normalize(instanceNormal * tangent);
	vec3 B = cross(N, T);

	gl_Position = camera.projectionView * surface;

	vertexOutput.surface = surface.xyz;
	vertexOutput.textureCoordinate = vec2(textureCoordinate.x, 1.0 - textureCoordinate.y);
	vertexOutput.TBN = mat3x3(T, B, N);
}