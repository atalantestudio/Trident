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

float4 main(Interstage interstage) : SV_Target {
	float value = 0.0;

	switch (uint(interstage.textureCoordinate.z)) {
		case 0: {
			value = userImages[0].Sample(userImageSamplerStates[0], interstage.textureCoordinate.xy).r;

			break;
		}
		case 1: {
			value = userImages[1].Sample(userImageSamplerStates[1], interstage.textureCoordinate.xy).r;

			break;
		}
		case 2: {
			value = userImages[2].Sample(userImageSamplerStates[2], interstage.textureCoordinate.xy).r;

			break;
		}
		case 3: {
			value = userImages[3].Sample(userImageSamplerStates[3], interstage.textureCoordinate.xy).r;

			break;
		}
		case 4: {
			value = userImages[4].Sample(userImageSamplerStates[4], interstage.textureCoordinate.xy).r;

			break;
		}
		case 5: {
			value = userImages[5].Sample(userImageSamplerStates[5], interstage.textureCoordinate.xy).r;

			break;
		}
		case 6: {
			value = userImages[6].Sample(userImageSamplerStates[6], interstage.textureCoordinate.xy).r;

			break;
		}
		case 7: {
			value = userImages[7].Sample(userImageSamplerStates[7], interstage.textureCoordinate.xy).r;

			break;
		}
		default: {
			break;
		}
	}

	float distanceEm = lerp(0.05, -0.05, value);
	float opacity = clamp(-distanceEm * interstage.fontSize * 3.0 + 0.5, 0.0, 1.0);

	return float4(interstage.color.rgb, interstage.color.a * opacity);
}