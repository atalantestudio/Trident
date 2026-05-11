// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Interstage {
	float4 position : SV_Position;
	nointerpolation float4 color : VERTEX_OUTPUT_ATTRIBUTE1;
	float3 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
	nointerpolation float fontSize : VERTEX_OUTPUT_ATTRIBUTE3;
};

Texture2DArray<float4> imageArray : register(t0);
SamplerState imageArraySamplerState : register(s0);

// See https://www.redblobgames.com/articles/sdf-fonts.
float calculateScreenScalePx(float2 textureCoordinate) {
	const float2 atlasSize = float2(2048.0, 2048.0);

	float2 gradient = fwidth(textureCoordinate);
	float2 product = atlasSize * gradient;

	return max(0.5 * dot(atlasSize, gradient) / (product.x * product.y), 1.0);
}

float4 main(Interstage interstage) : SV_Target {
	float screenScale = calculateScreenScalePx(interstage.textureCoordinate.xy);

	// NOTE: Looks good when multiplied by 3.0.
	float antialiasPerEm = interstage.fontSize * 3.0;

	float _sample = imageArray.Sample(imageArraySamplerState, interstage.textureCoordinate).r;
	float distanceEm = lerp(0.05, -0.05, _sample);
	float inverseWidth = screenScale * antialiasPerEm;
	float opacity = clamp(-distanceEm * inverseWidth + 0.5, 0.0, 1.0);

	return interstage.color * opacity;
}