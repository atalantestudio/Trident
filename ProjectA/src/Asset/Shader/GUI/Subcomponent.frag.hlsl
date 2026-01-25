struct VertexOutput {
	float4 position : SV_Position;
	float3 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE1;
	nointerpolation float4 color : VERTEX_OUTPUT_ATTRIBUTE2;
};

Texture2DArray<float4> imageArray : register(t0);
SamplerState imageArraySamplerState : register(s0);

float4 main(VertexOutput input) : SV_Target {
	return input.color * imageArray.Sample(imageArraySamplerState, input.textureCoordinate);
}