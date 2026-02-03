// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

static const float2 VERTICES[3] = {
	float2(-1.0,  1.0),
	float2( 3.0,  1.0),
	float2(-1.0, -3.0),
};

float4 main(uint vertexIndex : SV_VertexID) : SV_Position {
	return float4(VERTICES[vertexIndex], 0.0, 1.0);
}