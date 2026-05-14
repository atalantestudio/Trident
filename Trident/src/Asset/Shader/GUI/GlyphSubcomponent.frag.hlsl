// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Interstage {
	float4 position : SV_Position;
	nointerpolation float4 color : VERTEX_OUTPUT_ATTRIBUTE1;
	float3 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
	nointerpolation float fontSize : VERTEX_OUTPUT_ATTRIBUTE3;
};

Texture2D<float4> userImages[8] : register(t0);
SamplerState userImageSamplerStates[8] : register(s0);

// See https://www.redblobgames.com/articles/sdf-fonts.
float calculateScreenScalePx(float2 textureCoordinate) {
	const float2 atlasSize = float2(2048.0, 2048.0);

	float2 gradient = fwidth(textureCoordinate);
	float2 product = atlasSize * gradient;

	return max(0.5 * dot(atlasSize, gradient) / (product.x * product.y), 1.0);
}

float4 main(Interstage interstage) : SV_Target {
	float screenScale = calculateScreenScalePx(interstage.textureCoordinate.xy);
	float antialiasPerEm = interstage.fontSize;
	float _sample = 0.0;

	switch (uint(interstage.textureCoordinate.z)) {
		case 0: _sample = userImages[0].Sample(userImageSamplerStates[0], interstage.textureCoordinate.xy).r; break;
		case 1: _sample = userImages[1].Sample(userImageSamplerStates[1], interstage.textureCoordinate.xy).r; break;
		case 2: _sample = userImages[2].Sample(userImageSamplerStates[2], interstage.textureCoordinate.xy).r; break;
		case 3: _sample = userImages[3].Sample(userImageSamplerStates[3], interstage.textureCoordinate.xy).r; break;
		case 4: _sample = userImages[4].Sample(userImageSamplerStates[4], interstage.textureCoordinate.xy).r; break;
		case 5: _sample = userImages[5].Sample(userImageSamplerStates[5], interstage.textureCoordinate.xy).r; break;
		case 6: _sample = userImages[6].Sample(userImageSamplerStates[6], interstage.textureCoordinate.xy).r; break;
		case 7: _sample = userImages[7].Sample(userImageSamplerStates[7], interstage.textureCoordinate.xy).r; break;
	}

	float distanceEm = lerp(0.05, -0.05, _sample);

	// NOTE: Looks good when multiplied by something larger than 2.
	float inverseWidth = screenScale * antialiasPerEm * 2.5;

	float opacity = clamp(-distanceEm * inverseWidth + 0.5, 0.0, 1.0);

	return interstage.color * opacity;
}