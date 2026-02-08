// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct FragmentInput {
	float4 position : SV_Position;
	float3 surface : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
	float3x3 TBN : VERTEX_OUTPUT_ATTRIBUTE3;
};

struct FragmentOutput {
	float4 color : SV_Target;
};

struct Camera {
	float4x4 projectionView;
	float3 position;
};

struct Light {
	float3 position;
	float3 color;
};

Light createLight(float3 position, float3 color) {
	Light light;
	light.position = position;
	light.color = color;

	return light;
}

static const float PI = radians(180.0);
static const float3 AMBIENT_LIGHT = float3(0.03, 0.03, 0.03);
static const float DENOMINATOR_TOLERANCE = 0.0001;
static const uint LIGHT_COUNT = 2;
static const Light LIGHTS[LIGHT_COUNT] = {
	createLight(float3(-10.0, 10.0, -10.0), float3(300.0, 300.0, 300.0)),
	createLight(float3(5.0, 5.0, 10.0), float3(300.0, 300.0, 300.0)),
};

cbuffer Camera : register(b0) {
	Camera camera;
}

Texture2D<float4> armTexture : register(t7);
Texture2D<float4> diffuseTexture : register(t8);
Texture2D<float4> normalTexture : register(t9);

SamplerState armSamplerState : register(s0);
SamplerState diffuseSamplerState : register(s1);
SamplerState normalSamplerState : register(s2);

float TrowbridgeReitzGGX(float3 normal, float3 halfway, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float dotProduct = max(dot(normal, halfway), 0.0);
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

float Smith(float3 normal, float3 viewDirection, float3 lightDirection, float roughness) {
	float viewAngle = max(dot(normal, viewDirection), 0.0);
	float lightAngle = max(dot(normal, lightDirection), 0.0);

	float ggx1 = SchlickGGX(viewAngle, roughness);
	float ggx2 = SchlickGGX(lightAngle, roughness);

	return ggx1 * ggx2;
}

float3 FresnelSchlick(float angle, float3 baseReflectivity) {
	return baseReflectivity + (1.0 - baseReflectivity) * pow(clamp(1.0 - angle, 0.0, 1.0), 5.0);
}

FragmentOutput main(FragmentInput input) {
	float3 diffuse = pow(diffuseTexture.Sample(diffuseSamplerState, input.textureCoordinate).rgb, float3(2.2, 2.2, 2.2));
	float3 normal = normalize(mul(input.TBN, normalTexture.Sample(normalSamplerState, input.textureCoordinate).xyz * 2.0 - 1.0));
	float3 arm = armTexture.Sample(armSamplerState, input.textureCoordinate).rgb;
	float ao = arm[0];
	float roughness = arm[1];
	float metallic = arm[2];
	float3 surface = input.surface;
	float3 surfaceToViewDirection = normalize(camera.position - surface);
	float3 baseReflectivity = lerp(float3(0.04, 0.04, 0.04), diffuse, metallic);
	float3 L = float3(0.0, 0.0, 0.0);
	uint lightIndex = 0;

	while (lightIndex < LIGHT_COUNT) {
		Light light = LIGHTS[lightIndex];

		float3 surfaceToLightDirection = normalize(light.position - surface);
		float3 surfaceHalfway = normalize(surfaceToViewDirection + surfaceToLightDirection);
		float distance = length(light.position - surface);

		// Attenuate using (physically correct) inverse-square law.
		float attenuation = 1.0 / (distance * distance);

		float3 radiance = light.color * attenuation;

		float NDF = TrowbridgeReitzGGX(normal, surfaceHalfway, roughness);
		float G = Smith(normal, surfaceToViewDirection, surfaceToLightDirection, roughness);
		float3 F = FresnelSchlick(max(dot(surfaceHalfway, surfaceToViewDirection), 0.0), baseReflectivity);

		float3 kS = F;
		float3 kD = 1.0 - kS;

		// Cancel if the surface is metallic (metallic = 1.0).
		kD *= 1.0 - metallic;

		float3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(normal, surfaceToViewDirection), 0.0) * max(dot(normal, surfaceToLightDirection), 0.0) + DENOMINATOR_TOLERANCE;
		float3 specular = numerator / denominator;

		float angle = max(dot(normal, surfaceToLightDirection), 0.0);

		L += (kD * diffuse / PI + specular) * radiance * angle;

		lightIndex += 1;
	}

	float3 color = AMBIENT_LIGHT * diffuse * ao + L;

	// Apply HDR.
	color = color / (color + 1.0);
	color = pow(color, float3(1.0, 1.0, 1.0) / 2.2);

	FragmentOutput output;
	output.color = float4(color, 1.0);

	return output;
}