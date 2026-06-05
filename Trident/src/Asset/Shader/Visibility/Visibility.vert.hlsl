// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
};

struct Cluster {
	uint instanceIndex;
};

struct Instance {
	float4x4 world;
	float3x4 normal;
	uint geometryIndex;
	uint materialShaderIndex;
	uint materialIndex;
	int clusterOffset;
};

struct Geometry {
	uint vertexOffset;
	uint vertexTextureCoordinateOffset;
	uint clusterCount;
};

struct VertexInput {
	uint vertexIndex : SV_VertexID;
	uint instanceIndex : SV_InstanceID;
};

struct Interstage {
	float4 position : SV_Position;
	nointerpolation uint visibility : VERTEX_OUTPUT_ATTRIBUTE1;
};

static const uint CLUSTER_TRIANGLE_COUNT = 128;
static const uint CLUSTER_INDEX_COUNT = CLUSTER_TRIANGLE_COUNT * 3;
static const uint TRIANGLE_INDEX_BIT_COUNT = 7;

cbuffer CameraBuffer : register(b0) {
	Camera camera;
}

// NOTE
// These descriptors are part of a larger buffer.
// Because their stride differ, they can't be represented as structured buffers.
ByteAddressBuffer vertexPositionComponentBuffer : register(t0);
ByteAddressBuffer vertexPositionIndexBuffer : register(t1);
ByteAddressBuffer clusterBuffer : register(t2);
StructuredBuffer<Instance> instanceBuffer : register(t3);
ByteAddressBuffer geometryBuffer : register(t4);

Cluster loadCluster(uint clusterIndex) {
	Cluster cluster;
	cluster.instanceIndex = clusterBuffer.Load(clusterIndex * 4);

	return cluster;
}

Geometry loadGeometry(uint geometryIndex) {
	Geometry geometry;
	geometry.vertexOffset = geometryBuffer.Load(geometryIndex * 12 + 0);
	geometry.vertexTextureCoordinateOffset = geometryBuffer.Load(geometryIndex * 12 + 4);
	geometry.clusterCount = geometryBuffer.Load(geometryIndex * 12 + 8);

	return geometry;
}

float3 loadVertexPosition(Geometry geometry, uint index) {
	uint offset = (geometry.vertexOffset + index) * 3;

	return asfloat(vertexPositionComponentBuffer.Load3(offset * 4));
}

Interstage main(VertexInput input) {
	uint indexIndex = input.vertexIndex;
	uint triangleIndex = indexIndex / 3;
	uint clusterIndex = input.instanceIndex;

	Cluster cluster = loadCluster(clusterIndex);
	Instance instance = instanceBuffer[cluster.instanceIndex];
	Geometry geometry = loadGeometry(instance.geometryIndex);

	uint indexOffset = (instance.clusterOffset + clusterIndex) * CLUSTER_INDEX_COUNT;
	uint relativeVertexPositionIndex = vertexPositionIndexBuffer.Load((indexOffset + indexIndex) * 4);
	float3 vertex = loadVertexPosition(geometry, relativeVertexPositionIndex);

	Interstage output;
	output.position = mul(camera.projectionView, mul(instance.world, float4(vertex, 1.0)));
	output.visibility = ((clusterIndex + 1) << TRIANGLE_INDEX_BIT_COUNT) | triangleIndex;

	return output;
}