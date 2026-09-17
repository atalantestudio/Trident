#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

// TODO(forward/pbr)
// Decouple GLSL/GLSL for Vulkan shaders. Reasons:
// - Multiple descriptor sets
// - No shadow map Y flipping in GLSL

precision highp float;

struct PointLight {
	vec3 color;
	float intensity;
	vec3 position;
};

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
} camera;

layout(set = 0, binding = 2) buffer readonly PointLightBuffer {
	PointLight pointLights[];
};

layout(set = 0, binding = 3) uniform sampler2D directionalShadowMap;

layout(set = 1, binding = 0) uniform sampler2D lightmapAtlasMap;

layout(set = 2, binding = 0) uniform sampler2D armMap;
layout(set = 2, binding = 1) uniform sampler2D diffuseMap;
layout(set = 2, binding = 2) uniform sampler2D normalMap;

layout(location = 0) out vec4 fragment;

const float PI = radians(180.0);
const vec3 SUN_DIRECTION = normalize(vec3(1.0, 1.0, 1.0));

float TrowbridgeReitzGGX(vec3 normal, vec3 halfway, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float dotProduct = max(0.0, dot(normal, halfway));
	float dotProduct2 = dotProduct * dotProduct;
	float numerator = a2;

	float denominator = dotProduct2 * (a2 - 1.0) + 1.0;
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}

float SchlickGGX(float angle, float roughness) {
	// Remap roughness.
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;

	float numerator = angle;
	float denominator = angle * (1.0 - k) + k;

	return numerator / denominator;
}

float GeometrySmith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness) {
	float viewAngle = max(0.0, dot(normal, viewDirection));
	float lightAngle = max(0.0, dot(normal, lightDirection));
	float ggx1 = SchlickGGX(viewAngle, roughness);
	float ggx2 = SchlickGGX(lightAngle, roughness);

	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

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

float calculateShadow() {
	return 0.0;

	// TODO(renderer/forward): Restore.
	/* vec3 projectedPosition = lightSpacePosition.xyz / lightSpacePosition.w;

	if (projectedPosition.z < 0.0) {
		return 0.0;
	}

	vec2 coordinate = projectedPosition.xy * 0.5 + 0.5;
	float depthBias = 0.0005 * dot(interstage.tbn[2], SUN_DIRECTION);
	float depth = projectedPosition.z;

	coordinate.y = 1.0 - coordinate.y;

	float closestDepth = texture(directionalShadowMap, coordinate).r;

	return float(depth + depthBias <= closestDepth); */
}

void main() {
	float inShadow = calculateShadow();

	vec3 x = camera.position;
	vec3 wo = normalize(camera.position - interstage.surface);

	vec3 indirectDiffuse = sampleBicubic(lightmapAtlasMap, interstage.lightmapCoordinate).rgb;

	vec3 arm = texture(armMap, interstage.textureCoordinate).rgb;
	float ao = arm.r;
	float roughness = arm.g;
	float metallic = arm.b;
	vec3 c = texture(diffuseMap, interstage.textureCoordinate).rgb;
	vec3 n = normalize(interstage.tbn * (texture(normalMap, interstage.textureCoordinate).rgb * 2.0 - 1.0));
	float surfaceToSunLight = dot(n, SUN_DIRECTION);
	vec3 F0 = mix(vec3(0.04, 0.04, 0.04), c, metallic);

	vec3 Le = vec3(0.0, 0.0, 0.0);
	vec3 Lr = vec3(0.0, 0.0, 0.0);
	uint pointLightIndex = 0u;

	while (pointLightIndex < camera.pointLightCount) {
		PointLight pointLight = pointLights[pointLightIndex];

		vec3 surfaceToLight = pointLight.position - interstage.surface;
		vec3 wi = normalize(surfaceToLight);
		vec3 h = normalize(wo + wi);
		float d = dot(surfaceToLight, surfaceToLight);
		vec3 Li = (pointLight.color * pointLight.intensity) / d;

		vec3 F = FresnelSchlick(max(0.0, dot(wo, h)), F0);
		float D = TrowbridgeReitzGGX(n, h, roughness);
		float G = GeometrySmith(n, wo, wi, roughness);
		vec3 kd = vec3(1.0, 1.0, 1.0) - F * (1.0 - metallic);
		vec3 fr = kd * c / PI + ((D * F * G) / (4.0 * max(0.0, dot(wo, n)) * max(0.0, dot(wi, n)) + 1e-4));

		Lr += fr * Li * max(0.0, dot(wi, n));

		pointLightIndex += 1u;
	}

	vec3 Lo = Le + Lr;

	fragment.rgb = mix(indirectDiffuse * c * ao, c * camera.sunColor, max(0.0, max(0.0, surfaceToSunLight) - inShadow)) + Lo;
	fragment.a = 1.0;
}