#version 320 es

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;
precision highp sampler2DArray;

in Interstage {
	layout(location = 0) flat vec4 color;
	layout(location = 1) vec3 textureCoordinate;
} interstage;

layout(set = 0, binding = 1) uniform sampler2D userImages[8];

layout(location = 0) out vec4 fragment;

void main() {
	vec4 _sample = vec4(0.0, 0.0, 0.0, 1.0);

	switch (uint(interstage.textureCoordinate.z)) {
		case 0: _sample = texture(userImages[0], interstage.textureCoordinate.xy); break;
		case 1: _sample = texture(userImages[1], interstage.textureCoordinate.xy); break;
		case 2: _sample = texture(userImages[2], interstage.textureCoordinate.xy); break;
		case 3: _sample = texture(userImages[3], interstage.textureCoordinate.xy); break;
		case 4: _sample = texture(userImages[4], interstage.textureCoordinate.xy); break;
		case 5: _sample = texture(userImages[5], interstage.textureCoordinate.xy); break;
		case 6: _sample = texture(userImages[6], interstage.textureCoordinate.xy); break;
		case 7: _sample = texture(userImages[7], interstage.textureCoordinate.xy); break;
	}

	fragment = interstage.color * _sample;
}