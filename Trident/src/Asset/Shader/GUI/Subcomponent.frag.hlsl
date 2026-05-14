// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Interstage {
	float4 position : SV_Position;
	nointerpolation float4 color : VERTEX_OUTPUT_ATTRIBUTE1;
	float3 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
};

Texture2D<float4> userImages[8] : register(t0);
SamplerState userImageSamplerStates[8] : register(s0);

float4 main(Interstage interstage) : SV_Target {
	float4 _sample = float4(0.0, 0.0, 0.0, 1.0);

	switch (uint(interstage.textureCoordinate.z)) {
		case 0: _sample = userImages[0].Sample(userImageSamplerStates[0], interstage.textureCoordinate.xy); break;
		case 1: _sample = userImages[1].Sample(userImageSamplerStates[1], interstage.textureCoordinate.xy); break;
		case 2: _sample = userImages[2].Sample(userImageSamplerStates[2], interstage.textureCoordinate.xy); break;
		case 3: _sample = userImages[3].Sample(userImageSamplerStates[3], interstage.textureCoordinate.xy); break;
		case 4: _sample = userImages[4].Sample(userImageSamplerStates[4], interstage.textureCoordinate.xy); break;
		case 5: _sample = userImages[5].Sample(userImageSamplerStates[5], interstage.textureCoordinate.xy); break;
		case 6: _sample = userImages[6].Sample(userImageSamplerStates[6], interstage.textureCoordinate.xy); break;
		case 7: _sample = userImages[7].Sample(userImageSamplerStates[7], interstage.textureCoordinate.xy); break;
	}

	return interstage.color * _sample;
}