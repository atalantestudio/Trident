#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

// NOTE: "lowp" or "mediump" precision cause specular artifacts on integrated GPUs.
precision highp float;

in FragmentInput {
	layout(location = 0) vec3 surface;
	layout(location = 1) vec2 textureCoordinate;
	layout(location = 2) mat3x3 TBN;
} fragmentInput;

struct Light {
	vec3 position;
	vec3 color;
};

const float PI = radians(180.0);
const vec3 AMBIENT_LIGHT = vec3(0.03, 0.03, 0.03);
const float DENOMINATOR_TOLERANCE = 0.0001;
const uint LIGHT_COUNT = 2u;
const Light LIGHTS[LIGHT_COUNT] = Light[](
	Light(vec3(-10.0, 10.0, -10.0), vec3(300.0, 300.0, 300.0)),
	Light(vec3(5.0, 5.0, 10.0), vec3(300.0, 300.0, 300.0))
);

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
	vec3 position;
} camera;

layout(set = 1, binding = 0) uniform sampler2D aoTexture;
layout(set = 1, binding = 1) uniform sampler2D roughnessTexture;
layout(set = 1, binding = 2) uniform sampler2D diffuseTexture;
layout(set = 1, binding = 3) uniform sampler2D normalTexture;

layout(location = 0) out vec4 fragment;

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

float Smith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness) {
	float viewAngle = max(0.0, dot(normal, viewDirection));
	float lightAngle = max(0.0, dot(normal, lightDirection));
	float ggx1 = SchlickGGX(viewAngle, roughness);
	float ggx2 = SchlickGGX(lightAngle, roughness);

	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float angle, vec3 baseReflectivity) {
	return baseReflectivity + (1.0 - baseReflectivity) * pow(clamp(1.0 - angle, 0.0, 1.0), 5.0);
}

void main() {
	vec3 diffuse = pow(texture(diffuseTexture, fragmentInput.textureCoordinate).rgb, vec3(2.2, 2.2, 2.2));
	vec3 normal = normalize(fragmentInput.TBN * (texture(normalTexture, fragmentInput.textureCoordinate).xyz * 2.0 - 1.0));
	float ao = texture(aoTexture, fragmentInput.textureCoordinate).r;
	float roughness = texture(roughnessTexture, fragmentInput.textureCoordinate).r;
	float metallic = 0.0;
	vec3 surface = fragmentInput.surface;
	vec3 surfaceToViewDirection = normalize(camera.position - surface);
	vec3 baseReflectivity = mix(vec3(0.04, 0.04, 0.04), diffuse, metallic);
	vec3 L = vec3(0.0, 0.0, 0.0);
	uint lightIndex = 0u;

	while (lightIndex < LIGHT_COUNT) {
		Light light = LIGHTS[lightIndex];

		vec3 surfaceToLightDirection = normalize(light.position - surface);
		vec3 surfaceHalfway = normalize(surfaceToViewDirection + surfaceToLightDirection);
		float lightSurfaceDistance = length(light.position - surface);

		// Attenuate using (physically correct) inverse-square law.
		float attenuation = 1.0 / (lightSurfaceDistance * lightSurfaceDistance);

		vec3 radiance = light.color * attenuation;

		float NDF = TrowbridgeReitzGGX(normal, surfaceHalfway, roughness);
		float G = Smith(normal, surfaceToViewDirection, surfaceToLightDirection, roughness);
		vec3 F = FresnelSchlick(max(0.0, dot(surfaceHalfway, surfaceToViewDirection)), baseReflectivity);

		vec3 kS = F;
		vec3 kD = 1.0 - kS;

		// Cancel if the surface is metallic (metallic = 1.0).
		kD *= 1.0 - metallic;

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(0.0, dot(normal, surfaceToViewDirection)) * max(0.0, dot(normal, surfaceToLightDirection)) + DENOMINATOR_TOLERANCE;
		vec3 specular = numerator / denominator;
		float angle = max(0.0, dot(normal, surfaceToLightDirection));

		L += (kD * diffuse / PI + specular) * radiance * angle;

		lightIndex += 1u;
	}

	vec3 color = AMBIENT_LIGHT * diffuse * ao + L;

	// Apply HDR.
	color = color / (color + 1.0);
	color = pow(color, vec3(1.0 / 2.2));

	fragment = vec4(color, 1.0);
}