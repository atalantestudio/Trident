struct VertexOutput {
	float4 position : SV_Position;
	float3 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE1;
	nointerpolation float4 color : VERTEX_OUTPUT_ATTRIBUTE2;
};

Texture2DArray<float4> imageArray : register(t1);
SamplerState imageArraySamplerState : register(s1);

float4 main(VertexOutput input) : SV_Target {
	const float ALPHA_THRESHOLD = 0.5;
	const float SMOOTHNESS = 0.15;

	float value = smoothstep(ALPHA_THRESHOLD - SMOOTHNESS, ALPHA_THRESHOLD + SMOOTHNESS, imageArray.Sample(imageArraySamplerState, input.textureCoordinate).r);

	return value * input.color;
}