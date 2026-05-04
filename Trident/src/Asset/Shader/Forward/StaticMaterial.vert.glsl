#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in uvec2 index;

out Interstage {
	layout(location = 0) vec3 normal;
	layout(location = 1) vec2 textureCoordinate;
} interstage;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
} camera;

layout(set = 0, binding = 1) buffer readonly PositionBuffer {
	float positionBuffer[];
};

layout(set = 0, binding = 2) buffer readonly NormalBuffer {
	float normalBuffer[];
};

layout(set = 0, binding = 3) buffer readonly TextureCoordinateBuffer {
	float textureCoordinateBuffer[];
};

void main() {
	uint positionOffset = (index[0] & 0x00ffu) * 3u;
	uint normalOffset = (index[0] & 0xff00u) * 3u;
	uint textureCoordinateOffset = (index[1] & 0x00ffu) * 2u;

	vec3 position = vec3(
		positionBuffer[positionOffset + 0u],
		positionBuffer[positionOffset + 1u],
		positionBuffer[positionOffset + 2u]
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

	gl_Position = camera.projectionView * vec4(position, 1.0);

	interstage.normal = normal;
	interstage.textureCoordinate = textureCoordinate;
}