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
	uint materialIndex;
	uint materialReferenceIndex;
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

ByteAddressBuffer vertexPositionComponentBuffer : register(t0);
ByteAddressBuffer vertexPositionIndexBuffer : register(t1);
ByteAddressBuffer clusterBuffer : register(t2);

ByteAddressBuffer instanceBuffer : register(t3);
ByteAddressBuffer geometryBuffer : register(t4);

Cluster fetchCluster(uint clusterIndex) {
	static const uint stride = 4;

	Cluster cluster;
	cluster.instanceIndex = clusterBuffer.Load(clusterIndex * stride);

	return cluster;
}

Instance fetchInstance(uint instanceIndex) {
	static const uint stride = 80;

	Instance instance;
	instance.transform = transpose(float4x4(
		asfloat(instanceBuffer.Load4(instanceIndex * stride + 0)),
		asfloat(instanceBuffer.Load4(instanceIndex * stride + 16)),
		asfloat(instanceBuffer.Load4(instanceIndex * stride + 32)),
		asfloat(instanceBuffer.Load4(instanceIndex * stride + 48))
	));
	instance.geometryIndex = instanceBuffer.Load(instanceIndex * stride + 64);
	instance.materialIndex = instanceBuffer.Load(instanceIndex * stride + 68);
	instance.materialReferenceIndex = instanceBuffer.Load(instanceIndex * stride + 72);
	instance.clusterOffset = asint(instanceBuffer.Load(instanceIndex * stride + 76));

	return instance;
}

Geometry fetchGeometry(uint geometryIndex) {
	static const uint stride = 8;

	Geometry geometry;
	geometry.vertexOffset = geometryBuffer.Load(geometryIndex * stride + 0);
	geometry.clusterCount = geometryBuffer.Load(geometryIndex * stride + 4);

	return geometry;
}

float3 fetchVertexPosition(uint index, Geometry geometry) {
	uint offset = (geometry.vertexOffset + index) * VERTEX_POSITION_STRIDE;

	float x = asfloat(vertexPositionComponentBuffer.Load((offset + 0) * 4));
	float y = asfloat(vertexPositionComponentBuffer.Load((offset + 1) * 4));
	float z = asfloat(vertexPositionComponentBuffer.Load((offset + 2) * 4));

	return float3(x, y, z);
}

VertexOutput main(VertexInput input) {
	uint indexIndex = input.vertexIndex;
	uint triangleIndex = indexIndex / 3;
	uint clusterIndex = input.instanceIndex;

	Cluster cluster = fetchCluster(clusterIndex);
	Instance instance = fetchInstance(cluster.instanceIndex);
	Geometry geometry = fetchGeometry(instance.geometryIndex);

	uint indexOffset = (instance.clusterOffset + clusterIndex) * CLUSTER_INDEX_COUNT;
	uint relativeVertexPositionIndex = vertexPositionIndexBuffer.Load((indexOffset + indexIndex) * 4);
	float3 vertex = fetchVertexPosition(relativeVertexPositionIndex, geometry);

	VertexOutput output;
	output.position = mul(camera.projectionView, mul(instance.transform, float4(vertex, 1.0)));
	output.visibility = ((clusterIndex + 1) << TRIANGLE_INDEX_BIT_COUNT) | triangleIndex;

	return output;
}