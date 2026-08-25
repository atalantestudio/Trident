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

struct Interstage {
	float4 position : SV_Position;
	float3 normal : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
	float2 lightmapCoordinate : VERTEX_OUTPUT_ATTRIBUTE3;
	float4 lightSpacePosition : VERTEX_OUTPUT_ATTRIBUTE4;
};

struct FragmentOutput {
	float4 color : SV_Target;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

Texture2D<float4> lightmapAtlasTexture : register(t0);
SamplerState lightmapAtlasSamplerState : register(s0);

Texture2D<float> directionalShadowMapTexture : register(t1);
SamplerState directionalShadowMapSamplerState : register(s1);

Texture2D<float4> diffuseTexture : register(t2);
SamplerState diffuseSamplerState : register(s2);

float calculateShadow(float4 lightSpacePosition) {
	float3 projectedPosition = lightSpacePosition.xyz / lightSpacePosition.w;

	if (projectedPosition.z < 0.0) {
		return 0.0;
	}
	else {
		float2 coordinate = projectedPosition.xy * 0.5 + 0.5;
		coordinate.y = 1.0 - coordinate.y;

		float depthBias = 0.0005;
		float depth = projectedPosition.z;

		float closestDepth = directionalShadowMapTexture.Sample(directionalShadowMapSamplerState, coordinate);

		return depth + depthBias <= closestDepth;
	}
}

FragmentOutput main(Interstage interstage) {
	float inShadow = calculateShadow(interstage.lightSpacePosition);
	float3 directDiffuse = diffuseTexture.Sample(diffuseSamplerState, interstage.textureCoordinate).rgb;

	// TODO(forward/static): Sample the lightmap atlas with bicubic interpolation.
	float3 indirectDiffuse = lightmapAtlasTexture.Sample(lightmapAtlasSamplerState, interstage.lightmapCoordinate).rgb;

	directDiffuse = pow(directDiffuse, 1.0 / 2.2);

	FragmentOutput output;
	output.color.rgb = indirectDiffuse * directDiffuse * inShadow + directDiffuse * camera.sunColor * (1.0 - inShadow);
	output.color.a = 1.0;

	return output;
}