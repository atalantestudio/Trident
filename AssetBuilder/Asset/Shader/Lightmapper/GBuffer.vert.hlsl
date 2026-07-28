// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct VertexInput {
	uint vertexIndex : SV_VertexID;
	uint instanceIndex : SV_InstanceID;
	float2 lightmapCoordinate : VERTEX_INPUT_ATTRIBUTE0;
	uint4 index : VERTEX_INPUT_ATTRIBUTE1;
};

struct Interstage {
	float4 coordinate : SV_Position;
	float3 position : VERTEX_OUTPUT_ATTRIBUTE1;
	nointerpolation uint triangleIndex : VERTEX_OUTPUT_ATTRIBUTE2;
	nointerpolation float3 triangleNormal : VERTEX_OUTPUT_ATTRIBUTE3;
	float3 vertexNormal : VERTEX_OUTPUT_ATTRIBUTE4;
};

StructuredBuffer<float> positionBuffer : register(t0);
StructuredBuffer<float> vertexNormalBuffer : register(t1);

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
	uint vertexOffset = input.index[0] * 3;
	uint normalOffset = input.index[1] * 3;

	float3 position = float3(positionBuffer[vertexOffset + 0], positionBuffer[vertexOffset + 1], positionBuffer[vertexOffset + 2]);
	float3 vertexNormal = float3(vertexNormalBuffer[normalOffset + 0], vertexNormalBuffer[normalOffset + 1], vertexNormalBuffer[normalOffset + 2]);

	float2 coordinate = input.lightmapCoordinate;

	// TODO(asset-builder/lightmapper): Get the atlas size.
	coordinate += LIGHTMAP_COORDINATE_OFFSETS[input.instanceIndex] * 0.5 / 1024.0;

	coordinate.y = 1.0 - coordinate.y;

	Interstage interstage;
	interstage.coordinate = float4(coordinate * 2.0 - 1.0, 1.0, 1.0);
	interstage.position = position;
	interstage.triangleIndex = input.vertexIndex / 3 + 1;
	interstage.triangleNormal = vertexNormal;
	interstage.vertexNormal = vertexNormal;

	return interstage;
}