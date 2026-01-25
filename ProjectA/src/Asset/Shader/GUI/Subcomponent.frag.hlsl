struct VertexOutput {
	float4 position : SV_Position;
	nointerpolation float4 color : VERTEX_OUTPUT_ATTRIBUTE1;
	float3 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
};

Texture2DArray<float4> imageArray : register(t0);
SamplerState imageArraySamplerState : register(s0);

float4 main(VertexOutput input) : SV_Target {
	// return input.color;
	// return float4(input.textureCoordinate.xy, 0.0, 1.0);

	// TODO: Loading AND sampling with screen coordinates works but sampling with NDC coordinates doesn't work.
	// return input.color * imageArray.Load(int4(input.textureCoordinate.xy * int2(2048, 2048), input.textureCoordinate.z, 0));
	// return imageArray.Sample(imageArraySamplerState, float3(input.position.xy, input.textureCoordinate.z));
	return input.color * imageArray.Sample(imageArraySamplerState, input.textureCoordinate);
}