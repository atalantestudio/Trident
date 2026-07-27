// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint vertexIndex : SV_VertexID;
	float2 lightmapCoordinate : VERTEX_INPUT_ATTRIBUTE0;
	uint4 index : VERTEX_INPUT_ATTRIBUTE1;
};

struct Interstage {
	float4 position : SV_Position;
	nointerpolation uint triangleIndex : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 coordinate : VERTEX_OUTPUT_ATTRIBUTE2;
};

Interstage main(VertexInput input) {
	float2 coordinate = input.lightmapCoordinate;
	coordinate.y = 1.0 - coordinate.y;

	Interstage interstage;
	interstage.position = float4(coordinate * 2.0 - 1.0, 0.0, 1.0);
	interstage.triangleIndex = input.vertexIndex / 3;
	interstage.coordinate = input.lightmapCoordinate;

	return interstage;
}