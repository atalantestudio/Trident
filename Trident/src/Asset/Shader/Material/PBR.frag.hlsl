// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
	float3 position;
	uint pointLightCount;
	float3 ambientColor;
	uint _0;
	float3 sunColor;
};

struct PointLight {
	float3 color;
	float intensity;
	float3 position;
};

struct Interstage {
	float4 position : SV_Position;
	float3 surface : VERTEX_OUTPUT_ATTRIBUTE1;
	float3x3 tbn : VERTEX_OUTPUT_ATTRIBUTE2;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE5;
	float4 lightSpacePosition : VERTEX_OUTPUT_ATTRIBUTE6;
	nointerpolation uint instanceIndex : VERTEX_OUTPUT_ATTRIBUTE7;
};

static const float PI = radians(180.0);
static const float3 SUN_DIRECTION = normalize(float3(1.0, 1.0, 1.0));

cbuffer Camera : register(b0) {
	Camera camera;
}

StructuredBuffer<PointLight> pointLights : register(t0);

Texture2D<float> directionalShadowMapTexture : register(t1);
SamplerState directionalShadowMapSamplerState : register(s0);

Texture2D<float4> armTexture : register(t2);
SamplerState armSamplerState : register(s1);

Texture2D<float4> diffuseTexture : register(t3);
SamplerState diffuseSamplerState : register(s2);

Texture2D<float4> normalTexture : register(t4);
SamplerState normalSamplerState : register(s3);

float TrowbridgeReitzGGX(float3 normal, float3 halfway, float roughness) {
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

float GeometrySmith(float3 normal, float3 viewDirection, float3 lightDirection, float roughness) {
	float viewAngle = max(0.0, dot(normal, viewDirection));
	float lightAngle = max(0.0, dot(normal, lightDirection));
	float ggx1 = SchlickGGX(viewAngle, roughness);
	float ggx2 = SchlickGGX(lightAngle, roughness);

	return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float calculateShadow(float4 lightSpacePosition, float3 normal) {
	float3 projectedPosition = lightSpacePosition.xyz / lightSpacePosition.w;

	if (projectedPosition.z < 0.0) {
		return 0.0;
	}
	else {
		float2 coordinate = projectedPosition.xy * 0.5 + 0.5;
		coordinate.y = 1.0 - coordinate.y;

		float depthBias = 0.0005 * dot(normal, SUN_DIRECTION);
		float depth = projectedPosition.z;
		float closestDepth = directionalShadowMapTexture.Sample(directionalShadowMapSamplerState, coordinate);

		return depth + depthBias <= closestDepth;
	}
}

float4 main(Interstage interstage) : SV_Target0 {
	float inShadow = calculateShadow(interstage.lightSpacePosition, interstage.tbn[2]);

	float3 x = camera.position;
	float3 wo = normalize(camera.position - interstage.surface);

	float3 arm = armTexture.Sample(armSamplerState, interstage.textureCoordinate).rgb;
	float ao = arm.r;
	float roughness = arm.g;
	float metallic = arm.b;

	// TODO(pbr): Convert the albedo map to linear space while building?
	float3 c = pow(diffuseTexture.Sample(diffuseSamplerState, interstage.textureCoordinate).rgb, 2.2);

	float3 n = normalize(mul(transpose(interstage.tbn), normalTexture.Sample(normalSamplerState, interstage.textureCoordinate).rgb * 2.0 - 1.0));
	float surfaceToSunLight = dot(n, SUN_DIRECTION);
	float3 F0 = lerp(float3(0.04, 0.04, 0.04), c, metallic);

	float3 Le = float3(0.0, 0.0, 0.0);
	float3 Lr = float3(0.0, 0.0, 0.0);
	uint pointLightIndex = 0u;

	while (pointLightIndex < camera.pointLightCount) {
		PointLight pointLight = pointLights[pointLightIndex];

		float3 surfaceToLight = pointLight.position - interstage.surface;
		float3 wi = normalize(surfaceToLight);
		float3 h = normalize(wo + wi);
		float d = dot(surfaceToLight, surfaceToLight);
		float3 Li = (pointLight.color * pointLight.intensity) / d;

		float3 F = FresnelSchlick(max(0.0, dot(wo, h)), F0);
		float D = TrowbridgeReitzGGX(n, h, roughness);
		float G = GeometrySmith(n, wo, wi, roughness);
		float3 kd = float3(1.0, 1.0, 1.0) - F * (1.0 - metallic);
		float3 fr = kd * c / PI + ((D * F * G) / (4.0 * max(0.0, dot(wo, n)) * max(0.0, dot(wi, n)) + 1e-4));

		Lr += fr * Li * max(0.0, dot(wi, n));

		pointLightIndex += 1u;
	}

	float3 Lo = Le + Lr;

	float4 fragment;
	fragment.rgb = lerp(camera.ambientColor * c * ao, c * camera.sunColor, max(0.0, max(0.0, surfaceToSunLight) - inShadow)) + Lo;
	fragment.rgb = pow(fragment.rgb, 1.0 / 2.2);
	fragment.a = 1.0;

	return fragment;
}