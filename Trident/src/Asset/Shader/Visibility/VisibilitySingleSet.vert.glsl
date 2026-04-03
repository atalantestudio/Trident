#version 440

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

const uint CLUSTER_TRIANGLE_COUNT = 128;
const uint CLUSTER_INDEX_COUNT = CLUSTER_TRIANGLE_COUNT * 3;
const uint TRIANGLE_INDEX_BIT_COUNT = 7;
const uint VERTEX_POSITION_STRIDE = 3;

struct Camera {
	mat4x4 projectionView;
};

struct Cluster {
	uint instanceIndex;
};

struct Instance {
	mat4x4 transform;
	uint geometryIndex;
	uint materialShaderIndex;
	uint materialIndex;
	int clusterOffset;
};

struct Geometry {
	uint vertexOffset;
	uint clusterCount;
};

layout(binding = 0) uniform readonly CameraBuffer {
	Camera camera;
};
layout(binding = 1) buffer readonly VertexPositionComponentBuffer {
	float vertexPositionComponentBuffer[];
};
layout(binding = 2) buffer readonly VertexPositionIndexBuffer {
	uint vertexPositionIndexBuffer[];
};
layout(binding = 3) buffer readonly ClusterBuffer {
	Cluster clusterBuffer[];
};
layout(binding = 4) buffer readonly InstanceBuffer {
	Instance instanceBuffer[];
};
layout(binding = 5) buffer readonly GeometryBuffer {
	Geometry geometryBuffer[];
};

out VertexOutput {
	layout(location = 0) flat uint visibility;
} vertexOutput;

vec3 loadVertexPosition(uint index, Geometry geometry) {
	uint offset = (geometry.vertexOffset + index) * VERTEX_POSITION_STRIDE;

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
	vec3 vertex = loadVertexPosition(relativeVertexPositionIndex, geometry);

	gl_Position = camera.projectionView * instance.transform * vec4(vertex, 1.0);

	vertexOutput.visibility = ((clusterIndex + 1) << TRIANGLE_INDEX_BIT_COUNT) | triangleIndex;
}