// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

static const float2 VERTICES[3] = {
	float2(-1.0,  1.0),
	float2( 3.0,  1.0),
	float2(-1.0, -3.0),
};

struct VertexInput {
	uint vertexIndex : SV_VertexID;
};

struct Interstage {
	float4 position : SV_Position;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE1;
};

Interstage main(VertexInput input) {
	float2 vertex = VERTICES[input.vertexIndex];

	Interstage interstage;
	interstage.position = float4(vertex, 0.0, 1.0);
	interstage.textureCoordinate = vertex * 0.5 + 0.5;
	interstage.textureCoordinate.y = 1.0 - interstage.textureCoordinate.y;

	return interstage;
}