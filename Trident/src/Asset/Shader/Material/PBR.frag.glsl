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
	layout(location = 5) vec4 lightSpacePosition;
	layout(location = 6) flat uint instanceIndex;
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

layout(set = 0, binding = 3) uniform sampler2D directionalShadowMapSampler;

layout(set = 1, binding = 0) uniform sampler2D armTexture;
layout(set = 1, binding = 1) uniform sampler2D diffuseTexture;
layout(set = 1, binding = 2) uniform sampler2D normalTexture;

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

float calculateShadow(vec4 lightSpacePosition) {
	vec3 projectedPosition = lightSpacePosition.xyz / lightSpacePosition.w;

	if (projectedPosition.z < 0.0) {
		return 0.0;
	}

	vec2 coordinate = projectedPosition.xy * 0.5 + 0.5;
	float depthBias = 0.0005 * dot(interstage.tbn[2], SUN_DIRECTION);
	float depth = projectedPosition.z;

	coordinate.y = 1.0 - coordinate.y;

	float closestDepth = texture(directionalShadowMapSampler, coordinate).r;

	return float(depth + depthBias <= closestDepth);
}

void main() {
	float inShadow = calculateShadow(interstage.lightSpacePosition);

	vec3 x = camera.position;
	vec3 wo = normalize(camera.position - interstage.surface);

	vec3 arm = texture(armTexture, interstage.textureCoordinate).rgb;
	float ao = arm.r;
	float roughness = arm.g;
	float metallic = arm.b;

	// TODO(pbr): Convert the albedo map to linear space while building?
	vec3 c = pow(texture(diffuseTexture, interstage.textureCoordinate).rgb, vec3(2.2));

	vec3 n = normalize(interstage.tbn * (texture(normalTexture, interstage.textureCoordinate).rgb * 2.0 - 1.0));
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

	fragment.rgb = mix(camera.ambientColor * c * ao, c * camera.sunColor, max(0.0, max(0.0, surfaceToSunLight) - inShadow)) + Lo;
	fragment.rgb = pow(fragment.rgb, vec3(1.0 / 2.2));
	fragment.a = 1.0;
}