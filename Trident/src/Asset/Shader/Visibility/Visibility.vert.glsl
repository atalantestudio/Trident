#version 440

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

const uint CLUSTER_TRIANGLE_COUNT = 128;
const uint CLUSTER_INDEX_COUNT = CLUSTER_TRIANGLE_COUNT * 3;
const uint TRIANGLE_INDEX_BIT_COUNT = 7;

struct Camera {
	mat4x4 projectionView;
};

struct Cluster {
	uint instanceIndex;
};

struct Instance {
	mat4x4 world;
	mat3x3 normal;
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

layout(set = 0, binding = 0) uniform readonly CameraBuffer {
	Camera camera;
};
layout(set = 1, binding = 0) buffer readonly VertexPositionComponentBuffer {
	float vertexPositionComponentBuffer[];
};
layout(set = 1, binding = 1) buffer readonly VertexPositionIndexBuffer {
	uint vertexPositionIndexBuffer[];
};
layout(set = 1, binding = 2) buffer readonly ClusterBuffer {
	Cluster clusterBuffer[];
};
layout(set = 2, binding = 0) buffer readonly InstanceBuffer {
	Instance instanceBuffer[];
};
layout(set = 2, binding = 1) buffer readonly GeometryBuffer {
	Geometry geometryBuffer[];
};

out VertexOutput {
	layout(location = 0) flat uint visibility;
} vertexOutput;

vec3 loadVertexPosition(Geometry geometry, uint index) {
	uint offset = (geometry.vertexOffset + index) * 3;

	float x = vertexPositionComponentBuffer[offset + 0];
	float y = vertexPositionComponentBuffer[offset + 1];
	float z = vertexPositionComponentBuffer[offset + 2];

	return vec3(x, y, z);
}

void main() {
	uint indexIndex = gl_VertexIndex;
	uint triangleIndex = indexIndex / 3;
	uint clusterIndex = gl_InstanceIndex;

	Cluster cluster = clusterBuffer[clusterIndex];
	Instance instance = instanceBuffer[cluster.instanceIndex];
	Geometry geometry = geometryBuffer[instance.geometryIndex];

	uint indexOffset = (instance.clusterOffset + clusterIndex) * CLUSTER_INDEX_COUNT;
	uint relativeVertexPositionIndex = vertexPositionIndexBuffer[indexOffset + indexIndex];
	vec3 vertex = loadVertexPosition(geometry, relativeVertexPositionIndex);

	gl_Position = camera.projectionView * instance.world * vec4(vertex, 1.0);

	vertexOutput.visibility = ((clusterIndex + 1) << TRIANGLE_INDEX_BIT_COUNT) | triangleIndex;
}