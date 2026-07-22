#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

in Interstage {
	layout(location = 0) vec3 normal;
	layout(location = 1) vec2 textureCoordinate;
	layout(location = 2) vec2 lightmapCoordinate;
} interstage;

layout(set = 1, binding = 0) uniform sampler2D diffuseSampler;
layout(set = 1, binding = 1) uniform sampler2D lightmapAtlasSampler;

layout(location = 0) out vec4 fragment;

// See https://stackoverflow.com/questions/13501081/efficient-bicubic-filtering-code-in-glsl.
vec4 cubic(float v) {
	vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
	vec4 s = n * n * n;
	float x = s.x;
	float y = s.y - 4.0 * s.x;
	float z = s.z - 4.0 * s.y + 6.0 * s.x;
	float w = 6.0 - x - y - z;

	return vec4(x, y, z, w) * (1.0 / 6.0);
}

// See https://stackoverflow.com/questions/13501081/efficient-bicubic-filtering-code-in-glsl.
vec4 sampleBicubic(sampler2D textureSampler, vec2 coordinate) {
	vec2 textureSize = vec2(textureSize(textureSampler, 0));
	vec2 inverseTextureSize = 1.0 / textureSize;

	// [(0.0, 0.0), textureSize]
	coordinate *= textureSize;

	// [(-0.5, -0.5), textureSize - (0.5, 0.5)]
	coordinate -= vec2(0.5, 0.5);

	vec2 coordinateFraction = fract(coordinate);

	// [(-1.0, -1.0), textureSize - (1.0, 1.0)]
	coordinate -= coordinateFraction;

	vec4 cubicX = cubic(coordinateFraction.x);
	vec4 cubicY = cubic(coordinateFraction.y);

	vec4 c = coordinate.xxyy + vec2(-0.5, +1.5).xyxy;
	vec4 s = vec4(cubicX.xz + cubicX.yw, cubicY.xz + cubicY.yw);
	float sx = s.x / (s.x + s.y);
	float sy = s.z / (s.z + s.w);

	vec4 offset = (c + vec4(cubicX.yw, cubicY.yw) / s) * inverseTextureSize.xxyy;

	vec4 sample0 = texture(textureSampler, offset.xz);
	vec4 sample1 = texture(textureSampler, offset.yz);
	vec4 sample2 = texture(textureSampler, offset.xw);
	vec4 sample3 = texture(textureSampler, offset.yw);

	return mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy);
}

void main() {
	vec3 diffuse = texture(diffuseSampler, interstage.textureCoordinate).rgb;
	vec3 light = sampleBicubic(lightmapAtlasSampler, interstage.lightmapCoordinate).rgb;

	fragment.rgb = diffuse * light;

	fragment.rgb = pow(fragment.rgb, vec3(1.0 / 2.2));
	fragment.a = 1.0;
}