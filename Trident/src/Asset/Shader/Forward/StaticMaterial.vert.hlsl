// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
};

struct Vertex {
	uint index;
	uint textureCoordinateIndex;
	uint lightmapCoordinateIndex;
};

struct Triangle {
	Vertex vertices[3];
	uint normalIndex;
};

struct VertexInput {
	uint vertexIndex : SV_VertexID;
};

struct Interstage {
	float4 position : SV_Position;
	float3 normal : VERTEX_OUTPUT_ATTRIBUTE1;
	float2 textureCoordinate : VERTEX_OUTPUT_ATTRIBUTE2;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

StructuredBuffer<Triangle> triangleBuffer : register(t0);
StructuredBuffer<float> vertexBuffer : register(t1);
StructuredBuffer<float> normalBuffer : register(t2);
StructuredBuffer<float> textureCoordinateBuffer : register(t3);

Interstage main(VertexInput input) {
	uint triangleIndex = input.vertexIndex / 3;
	uint triangleVertexIndex = input.vertexIndex - triangleIndex * 3;

	Triangle triangle_ = triangleBuffer[triangleIndex];
	Vertex triangleVertex = triangle_.vertices[triangleVertexIndex];

	uint vertexOffset = triangleVertex.index * 3;
	uint normalOffset = triangle_.normalIndex * 3;
	uint textureCoordinateOffset = triangleVertex.textureCoordinateIndex * 2;

	float3 vertex = float3(
		vertexBuffer[vertexOffset + 0],
		vertexBuffer[vertexOffset + 1],
		vertexBuffer[vertexOffset + 2]
	);
	float3 normal = float3(
		normalBuffer[normalOffset + 0],
		normalBuffer[normalOffset + 1],
		normalBuffer[normalOffset + 2]
	);
	float2 textureCoordinate = float2(
		textureCoordinateBuffer[textureCoordinateOffset + 0],
		textureCoordinateBuffer[textureCoordinateOffset + 1]
	);

	Interstage interstage;
	interstage.position = mul(camera.projectionView, float4(vertex, 1.0));
	interstage.normal = normal;
	interstage.textureCoordinate = textureCoordinate;

	return interstage;
}