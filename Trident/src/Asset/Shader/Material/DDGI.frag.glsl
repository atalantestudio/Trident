#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

// #define CUSTOM_SCENE_DDGI_REF_0

precision highp float;

in Interstage {
	layout(location = 0) vec3 surface;
	layout(location = 1) mat3x3 tbn;
	layout(location = 4) vec2 textureCoordinate;
	layout(location = 5) vec2 lightmapCoordinate;
} interstage;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
	vec3 position;
	uint pointLightCount;
} camera;

layout(set = 1, binding = 0) uniform sampler2D lightmapAtlasMap;

layout(set = 2, binding = 0) uniform sampler2D diffuseMap;
layout(set = 2, binding = 1) uniform sampler2D normalMap;

layout(location = 0) out vec4 fragment;

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

float sdSphere(vec3 position, float radius) {
	return length(position) - radius;
}

float sdBox(vec3 position, vec3 size) {
	vec3 q = abs(position) - size / 2.0;

	return length(max(vec3(0.0, 0.0, 0.0), q)) + min(0.0, max(q.x, max(q.y, q.z)));
}

void main() {
#if defined(CUSTOM_SCENE_DDGI_REF_0)
	fragment.rgb = sampleBicubic(lightmapAtlasMap, interstage.lightmapCoordinate).rgb;
#else
	const vec3 lightPosition = vec3(0.0, 64.0, 164.0);
	const float lightIntensity = 256.0;

	const vec3 boxPosition = vec3(32.0, 32.0, 196.0);
	const vec3 boxSize = vec3(32.0, 32.0, 32.0);

	vec3 p = boxPosition - interstage.surface;

	float d = sdBox(p, boxSize);

	if (d <= 0.0) {
		fragment.rgb = vec3(1.0);

		return;
	}

	vec3 surfaceToLight = lightPosition - interstage.surface;

	float attenuation = dot(surfaceToLight, surfaceToLight);
	float light = lightIntensity / attenuation;

	fragment.rgb = vec3(light);
	fragment.rgb = pow(fragment.rgb, vec3(1.0 / 2.2));
#endif

	fragment.a = 1.0;
}