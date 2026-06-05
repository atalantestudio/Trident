// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Vertex {
	uint vertexIndex;
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

struct Camera {
	float4x4 projectionView;
};

cbuffer Camera : register(b0) {
	Camera camera;
}

ByteAddressBuffer triangleBuffer : register(t0);
ByteAddressBuffer vertexBuffer : register(t1);
ByteAddressBuffer normalBuffer : register(t2);
ByteAddressBuffer textureCoordinateBuffer : register(t3);

Triangle loadTriangle(uint triangleIndex) {
	static const uint VERTEX_SIZE = 3 * 4;
	static const uint TRIANGLE_SIZE = VERTEX_SIZE * 3 + 4;

	uint triangleOffset = triangleIndex * TRIANGLE_SIZE;

	Triangle triangle_;
	triangle_.vertices[0].vertexIndex = triangleBuffer.Load(triangleOffset + 0 * VERTEX_SIZE + 0);
	triangle_.vertices[0].textureCoordinateIndex = triangleBuffer.Load(triangleOffset + 0 * VERTEX_SIZE + 4);
	triangle_.vertices[0].lightmapCoordinateIndex = triangleBuffer.Load(triangleOffset + 0 * VERTEX_SIZE + 8);
	triangle_.vertices[1].vertexIndex = triangleBuffer.Load(triangleOffset + 1 * VERTEX_SIZE + 0);
	triangle_.vertices[1].textureCoordinateIndex = triangleBuffer.Load(triangleOffset + 1 * VERTEX_SIZE + 4);
	triangle_.vertices[1].lightmapCoordinateIndex = triangleBuffer.Load(triangleOffset + 1 * VERTEX_SIZE + 8);
	triangle_.vertices[2].vertexIndex = triangleBuffer.Load(triangleOffset + 2 * VERTEX_SIZE + 0);
	triangle_.vertices[2].textureCoordinateIndex = triangleBuffer.Load(triangleOffset + 2 * VERTEX_SIZE + 4);
	triangle_.vertices[2].lightmapCoordinateIndex = triangleBuffer.Load(triangleOffset + 2 * VERTEX_SIZE + 8);

	triangle_.normalIndex = triangleBuffer.Load(triangleOffset + 3 * VERTEX_SIZE);

	return triangle_;
}

Interstage main(VertexInput input) {
	uint triangleIndex = input.vertexIndex / 3;
	uint triangleVertexIndex = input.vertexIndex - triangleIndex * 3;

	Triangle triangle_ = loadTriangle(triangleIndex);
	Vertex triangleVertex = triangle_.vertices[triangleVertexIndex];

	uint vertexOffset = triangleVertex.vertexIndex * 3 * 4;
	uint normalOffset = triangle_.normalIndex * 3 * 4;
	uint textureCoordinateOffset = triangleVertex.textureCoordinateIndex * 2 * 4;

	float3 vertex = asfloat(vertexBuffer.Load3(vertexOffset));
	float3 normal = asfloat(normalBuffer.Load3(normalOffset));
	float2 textureCoordinate = asfloat(textureCoordinateBuffer.Load2(textureCoordinateOffset));

	Interstage interstage;
	interstage.position = mul(camera.projectionView, float4(vertex, 1.0));
	interstage.normal = normal;
	interstage.textureCoordinate = textureCoordinate;

	return interstage;
}