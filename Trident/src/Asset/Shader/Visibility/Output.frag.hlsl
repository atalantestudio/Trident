// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

Texture2D<float4> outputImage : register(t0);

float4 main(float4 position : SV_Position) : SV_Target {
	return outputImage[position.xy];
}