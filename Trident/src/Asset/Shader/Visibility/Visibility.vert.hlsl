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

StructuredBuffer<float> vertexPositionComponentBuffer : register(t0);
StructuredBuffer<uint> vertexPositionIndexBuffer : register(t1);
StructuredBuffer<Cluster> clusterBuffer : register(t2);
StructuredBuffer<Instance> instanceBuffer : register(t3);
StructuredBuffer<Geometry> geometryBuffer : register(t4);

float3 loadVertexPosition(Geometry geometry, uint index) {
	uint offset = (geometry.vertexOffset + index) * 3;

	return float3(
		vertexPositionComponentBuffer[offset + 0],
		vertexPositionComponentBuffer[offset + 1],
		vertexPositionComponentBuffer[offset + 2]
	);
}

Interstage main(VertexInput input) {
	uint indexIndex = input.vertexIndex;
	uint triangleIndex = indexIndex / 3;
	uint clusterIndex = input.instanceIndex;

	Cluster cluster = clusterBuffer[clusterIndex];
	Instance instance = instanceBuffer[cluster.instanceIndex];
	Geometry geometry = geometryBuffer[instance.geometryIndex];

	uint indexOffset = (instance.clusterOffset + clusterIndex) * CLUSTER_INDEX_COUNT;
	uint relativeVertexPositionIndex = vertexPositionIndexBuffer[indexOffset + indexIndex];
	float3 vertex = loadVertexPosition(geometry, relativeVertexPositionIndex);

	Interstage interstage;
	interstage.position = mul(camera.projectionView, mul(instance.world, float4(vertex, 1.0)));
	interstage.visibility = ((clusterIndex + 1) << TRIANGLE_INDEX_BIT_COUNT) | triangleIndex;

	return interstage;
}