// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
	float3 position;
	uint pointLightCount;
	float3 ambientColor;
	float farPlane;
	float3 sunColor;
};

#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
	struct DirectionalLight {
		float4x4 projectionViews[3];
	};
#endif

struct Interstage {
	float4 position : SV_Position;
	float3 surface : VERTEX_OUTPUT_ATTRIBUTE1;
	float3x3 tbn : VERTEX_OUTPUT_ATTRIBUTE2;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE5;
	float4 lightSpacePosition : VERTEX_OUTPUT_ATTRIBUTE6;
	float2 lightmapCoordinate : VERTEX_OUTPUT_ATTRIBUTE7;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
	cbuffer DirectionalLight : register(b2) {
		DirectionalLight directionalLight;
	}
#endif

#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
	Texture2D<float> directionalShadowMapTexture : register(t1);
	SamplerState directionalShadowMapSamplerState : register(s0);
#endif

Texture2D<float4> lightmapAtlasTexture : register(t2);
SamplerState lightmapAtlasSamplerState : register(s1);

Texture2D<float4> diffuseTexture : register(t3);
SamplerState diffuseSamplerState : register(s2);

Texture2D<float4> normalTexture : register(t4);
SamplerState normalSamplerState : register(s3);

static const float3 SUN_DIRECTION = normalize(float3(1.0, 1.0, 1.0));

#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
	float calculateShadow(float3 normal) {
		return 0.0;

		// TODO(renderer/forward): Restore.
		/* float3 projectedPosition = lightSpacePosition.xyz / lightSpacePosition.w;

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
		} */
	}
#endif

float4 main(Interstage interstage) : SV_Target0 {
	#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
		float inShadow = calculateShadow(interstage.tbn[2]);
	#else
		float inShadow = 1.0;
	#endif

	// TODO(graphics/dx12/sampler-descriptor-heap): Fix wrong sampler being used.
	float3 directDiffuse = diffuseTexture.Sample(diffuseSamplerState, interstage.textureCoordinate).rgb;

	// TODO(forward/static): Sample the lightmap atlas with bicubic interpolation.
	float3 indirectDiffuse = lightmapAtlasTexture.Sample(lightmapAtlasSamplerState, interstage.lightmapCoordinate).rgb;

	float3 normal = normalize(mul(transpose(interstage.tbn), normalTexture.Sample(normalSamplerState, interstage.textureCoordinate).rgb * 2.0 - 1.0));
	float surfaceToSunLight = dot(normal, SUN_DIRECTION);

	directDiffuse = pow(directDiffuse, 1.0 / 2.2);

	float4 fragment;
	fragment.rgb = lerp(indirectDiffuse * directDiffuse, directDiffuse * camera.sunColor, max(0.0, max(0.0, surfaceToSunLight) - inShadow));
	fragment.a = 1.0;

	return fragment;
}