#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;
precision highp sampler2DArray;

in Interstage {
	layout(location = 0) flat vec4 color;
	layout(location = 1) vec3 textureCoordinate;
	layout(location = 2) flat float fontSize;
} interstage;

layout(set = 0, binding = 1) uniform sampler2D userImages[8];

layout(location = 0) out vec4 fragment;

void main() {
	float value = 0.0;

	switch (uint(interstage.textureCoordinate.z)) {
		case 0: {
			value = texture(userImages[0], interstage.textureCoordinate.xy).r;

			break;
		}
		case 1: {
			value = texture(userImages[1], interstage.textureCoordinate.xy).r;

			break;
		}
		case 2: {
			value = texture(userImages[2], interstage.textureCoordinate.xy).r;

			break;
		}
		case 3: {
			value = texture(userImages[3], interstage.textureCoordinate.xy).r;

			break;
		}
		case 4: {
			value = texture(userImages[4], interstage.textureCoordinate.xy).r;

			break;
		}
		case 5: {
			value = texture(userImages[5], interstage.textureCoordinate.xy).r;

			break;
		}
		case 6: {
			value = texture(userImages[6], interstage.textureCoordinate.xy).r;

			break;
		}
		case 7: {
			value = texture(userImages[7], interstage.textureCoordinate.xy).r;

			break;
		}
		default: {
			break;
		}
	}

	float distanceEm = mix(0.05, -0.05, value);
	float opacity = clamp(-distanceEm * interstage.fontSize * 3.0 + 0.5, 0.0, 1.0);

	fragment = vec4(interstage.color.rgb, interstage.color.a * opacity);
}