#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

// TODO(renderer/forward)
// Decouple GLSL/GLSL for Vulkan shaders. Reasons:
// - Multiple descriptor sets
// - No shadow map Y flipping in GLSL

// #define DEBUG_VISUALIZE_DIRECTIONAL_SHADOW_MAP_CASCADES

precision highp float;
precision highp sampler2DArray;

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
	vec3 ambientColor;
	uint _0;
	vec3 sunColor;
	uint _1;
	mat4x4 skyboxProjectionView;
	mat4x4 view;
} camera;

layout(set = 0, binding = 1) uniform readonly DirectionalLight {
	mat4x4 projectionViews[3];
} directionalLight;

layout(set = 0, binding = 3) uniform sampler2DArray directionalShadowMap;

layout(set = 1, binding = 0) uniform sampler2D lightmapAtlasMap;

layout(set = 2, binding = 0) uniform sampler2D diffuseMap;
layout(set = 2, binding = 1) uniform sampler2D normalMap;

layout(location = 0) out vec4 fragment;

const vec3 SUN_DIRECTION = normalize(vec3(1.0, 1.0, 1.0));
const uint CASCADE_COUNT = 3u;
const float CASCADE_FAR_PLANES[CASCADE_COUNT] = float[](0.333, 0.666, 1.0);
const float CAMERA_FAR_PLANE = 2048.0;

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

uint findCascadeIndex(vec3 surface) {
	vec4 viewSpaceSurface = camera.view * vec4(surface, 1.0);
	float surfaceViewDepth = abs(viewSpaceSurface.z);
	uint cascadeIndex = 0u;

	while (cascadeIndex < CASCADE_COUNT) {
		if (surfaceViewDepth < CAMERA_FAR_PLANE * CASCADE_FAR_PLANES[cascadeIndex]) {
			break;
		}

		cascadeIndex += 1u;
	}

	return cascadeIndex;
}

float calculateShadow(vec3 surface, vec3 normal, uint cascadeIndex) {
	if (cascadeIndex >= CASCADE_COUNT) {
		return 0.0;
	}

	vec4 lightSpaceSurface = directionalLight.projectionViews[cascadeIndex] * vec4(surface, 1.0);
	vec3 projectedPosition = lightSpaceSurface.xyz / lightSpaceSurface.w;

	if (projectedPosition.z > 1.0) {
		return 0.0;
	}

	vec2 coordinate = projectedPosition.xy * 0.5 + 0.5;

	// float depthBias = max(0.005, 0.05 * (1.0 - dot(normalize(normal), SUN_DIRECTION)));
	float depthBias = max(0.05 * (1.0 - dot(normal, SUN_DIRECTION)), 0.005);
	// float depthBias = 0.0005 * dot(normal, SUN_DIRECTION);
	depthBias *= 1.0 / (CAMERA_FAR_PLANE * CASCADE_FAR_PLANES[cascadeIndex] * 0.5);

	float depth = projectedPosition.z;

	coordinate.y = 1.0 - coordinate.y;

	float closestDepth = texture(directionalShadowMap, vec3(coordinate, float(cascadeIndex))).r;

	return float(depth - depthBias > closestDepth);
}

void main() {
	uint cascadeIndex = findCascadeIndex(interstage.surface);
	float inShadow = calculateShadow(interstage.surface, interstage.tbn[2], cascadeIndex);

	vec3 directDiffuse = texture(diffuseMap, interstage.textureCoordinate).rgb;
	vec3 indirectDiffuse = sampleBicubic(lightmapAtlasMap, interstage.lightmapCoordinate).rgb;
	vec3 normal = normalize(interstage.tbn * (texture(normalMap, interstage.textureCoordinate).rgb * 2.0 - 1.0));
	float surfaceToSunLight = dot(normal, SUN_DIRECTION);

	directDiffuse = pow(directDiffuse, vec3(1.0 / 2.2));

	vec3 shadow = indirectDiffuse * directDiffuse;

	#if defined(DEBUG_VISUALIZE_DIRECTIONAL_SHADOW_MAP_CASCADES)
		vec3 directionalShadowMapCascadeColor = vec3(0.0, 0.0, 0.0);

		if (cascadeIndex == 0u) {
			directionalShadowMapCascadeColor.r = 1.0;
		}
		else if (cascadeIndex == 1u) {
			directionalShadowMapCascadeColor.g = 1.0;
		}
		else if (cascadeIndex == 2u) {
			directionalShadowMapCascadeColor.b = 1.0;
		}

		shadow *= directionalShadowMapCascadeColor;
	#endif

	fragment.rgb = mix(shadow, directDiffuse * camera.sunColor, max(0.0, max(0.0, surfaceToSunLight) - inShadow));
	fragment.a = 1.0;
}