// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint vertexIndex : SV_VertexID;
	uint instanceIndex : SV_InstanceID;
	uint index : VERTEX_INPUT_ATTRIBUTE0;
};

struct Interstage {
	float4 coordinate : SV_Position;
	float3 position : VERTEX_OUTPUT_ATTRIBUTE1;
	nointerpolation uint triangleIndex : VERTEX_OUTPUT_ATTRIBUTE2;
	nointerpolation float3 triangleNormal : VERTEX_OUTPUT_ATTRIBUTE3;
	float3 vertexNormal : VERTEX_OUTPUT_ATTRIBUTE4;
};

struct Vertex {
	float3 position;
	float3 normal;
	float3 tangent;
	float2 lightmapCoordinate;
};

StructuredBuffer<Vertex> vertexBuffer : register(t0);

static const float2 LIGHTMAP_COORDINATE_OFFSETS[25] = {
	float2(-2.0, -2.0),
	float2( 2.0, -2.0),
	float2(-2.0,  2.0),
	float2( 2.0,  2.0),
	float2(-1.0, -2.0),
	float2( 1.0, -2.0),
	float2(-2.0, -1.0),
	float2( 2.0, -1.0),
	float2(-2.0,  1.0),
	float2( 2.0,  1.0),
	float2(-1.0,  2.0),
	float2( 1.0,  2.0),
	float2(-2.0,  0.0),
	float2( 2.0,  0.0),
	float2( 0.0, -2.0),
	float2( 0.0,  2.0),
	float2(-1.0, -1.0),
	float2( 1.0, -1.0),
	float2(-1.0,  0.0),
	float2( 1.0,  0.0),
	float2(-1.0,  1.0),
	float2( 1.0,  1.0),
	float2( 0.0, -1.0),
	float2( 0.0,  1.0),
	float2( 0.0,  0.0),
};

Interstage main(VertexInput input) {
	Vertex vertex = vertexBuffer[input.index];

	float2 coordinate = vertex.lightmapCoordinate;

	// TODO(asset-builder/lightmapper): Get the atlas size.
	coordinate += LIGHTMAP_COORDINATE_OFFSETS[input.instanceIndex] * 0.5 / 1024.0;

	coordinate.y = 1.0 - coordinate.y;

	Interstage interstage;
	interstage.coordinate = float4(coordinate * 2.0 - 1.0, 1.0, 1.0);
	interstage.position = vertex.position;
	interstage.triangleIndex = input.vertexIndex / 3 + 1;
	interstage.triangleNormal = vertex.normal;
	interstage.vertexNormal = vertex.normal;

	return interstage;
}