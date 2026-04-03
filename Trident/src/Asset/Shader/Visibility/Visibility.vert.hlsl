// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
};

struct Cluster {
	uint instanceIndex;
};

struct Instance {
	float4x4 transform;
	uint geometryIndex;
	uint materialShaderIndex;
	uint materialIndex;
	int clusterOffset;
};

struct Geometry {
	uint vertexOffset;
	uint clusterCount;
};

struct VertexInput {
	uint vertexIndex : SV_VertexID;
	uint instanceIndex : SV_InstanceID;
};

struct VertexOutput {
	float4 position : SV_Position;
	nointerpolation uint visibility : VERTEX_OUTPUT_ATTRIBUTE1;
};

static const uint CLUSTER_TRIANGLE_COUNT = 128;
static const uint CLUSTER_INDEX_COUNT = CLUSTER_TRIANGLE_COUNT * 3;
static const uint TRIANGLE_INDEX_BIT_COUNT = 7;
static const uint VERTEX_POSITION_STRIDE = 3;

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
	static const uint stride = 4;

	Cluster cluster;
	cluster.instanceIndex = clusterBuffer.Load(clusterIndex * stride);

	return cluster;
}

Geometry loadGeometry(uint geometryIndex) {
	static const uint stride = 8;

	Geometry geometry;
	geometry.vertexOffset = geometryBuffer.Load(geometryIndex * stride + 0);
	geometry.clusterCount = geometryBuffer.Load(geometryIndex * stride + 4);

	return geometry;
}

float3 loadVertexPosition(uint index, Geometry geometry) {
	uint offset = (geometry.vertexOffset + index) * VERTEX_POSITION_STRIDE;

	return asfloat(vertexPositionComponentBuffer.Load3(offset * 4));
}

VertexOutput main(VertexInput input) {
	uint indexIndex = input.vertexIndex;
	uint triangleIndex = indexIndex / 3;
	uint clusterIndex = input.instanceIndex;

	Cluster cluster = loadCluster(clusterIndex);
	Instance instance = instanceBuffer[cluster.instanceIndex];
	Geometry geometry = loadGeometry(instance.geometryIndex);

	uint indexOffset = (instance.clusterOffset + clusterIndex) * CLUSTER_INDEX_COUNT;
	uint relativeVertexPositionIndex = vertexPositionIndexBuffer.Load((indexOffset + indexIndex) * 4);
	float3 vertex = loadVertexPosition(relativeVertexPositionIndex, geometry);

	VertexOutput output;
	output.position = mul(camera.projectionView, mul(instance.transform, float4(vertex, 1.0)));
	output.visibility = ((clusterIndex + 1) << TRIANGLE_INDEX_BIT_COUNT) | triangleIndex;

	return output;
}