struct VertexInput {
	float4 vertexAndTextureCoordinate : VERTEX_INPUT_ATTRIBUTE0;
	float3x3 worldTransform : VERTEX_INPUT_ATTRIBUTE1;
	float3x3 textureTransform : VERTEX_INPUT_ATTRIBUTE4;
	uint layerIndex : VERTEX_INPUT_ATTRIBUTE7;
	float4 color : VERTEX_INPUT_ATTRIBUTE8;
};

struct Camera {
	float3x3 projection;
};

struct VertexOutput {
	float4 position : SV_Position;
	float3 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE1;
	nointerpolation float4 color : VERTEX_OUTPUT_ATTRIBUTE2;
};

Camera camera : register(c0);

VertexOutput main(VertexInput input) {
	float3 vertex = float3(input.vertexAndTextureCoordinate.xy, 1.0);
	float3 textureCoordinate = float3(input.vertexAndTextureCoordinate.zw, 1.0);

	VertexOutput output;
	output.position = float4(mul(mul(camera.projection, input.worldTransform), vertex), 1.0);
	output.textureCoordinate = mul(input.textureTransform, textureCoordinate);
	output.textureCoordinate.z = float(input.layerIndex);
	output.color = input.color;

	return output;
}