// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint vertexIndex : SV_VertexID;
};

struct VertexOutput {
	float4 position : SV_Position;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE1;
};

static const float2 VERTICES[3] = {
	float2(-1.0,  1.0),
	float2( 3.0,  1.0),
	float2(-1.0, -3.0),
};

VertexOutput main(VertexInput input) {
	float2 vertex = VERTICES[input.vertexIndex];

	VertexOutput output;
	output.position = float4(vertex, 0.0, 1.0);
	output.textureCoordinate = vertex * 0.5 + 0.5;

	return output;
}