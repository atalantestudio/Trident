#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Vertex {
	float x;
	float y;
	float z;
	uint normal;
	uint tangent;
	uint textureCoordinate;
};

struct GIVertex {
	vec4 position;
	vec4 a;
	vec4 b;
	vec4 c;
};

struct PointLight {
	vec3 color;
	float intensity;
	vec3 position;
};

struct Plane {
	vec3 offset;
	vec3 normal;
};

struct DrawIndirectCommand {
	uint vertexCount;
	uint instanceCount;
	uint vertexOffset;
	uint instanceOffset;
};

const vec3 LIGHT_NORMAL = vec3(0.0, -1.0, 0.0);
const vec2 LIGHT_SIZE = vec2(16.0, 16.0);

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(set = 0, binding = 1) buffer readonly VertexBuffer {
	Vertex vertices[];
};

layout(set = 0, binding = 2) buffer readonly IndexBuffer {
	uint indices[];
};

layout(set = 0, binding = 3) buffer readonly PointLightBuffer {
	PointLight pointLights[];
};

layout(set = 0, binding = 4) buffer writeonly GIVertexBuffer {
	GIVertex giVertices[];
};

layout(set = 0, binding = 5) buffer GIIndirectBuffer {
	DrawIndirectCommand drawCommand;
};

vec3 loadVertexPosition(uint index) {
	Vertex vertex = vertices[index];

	return vec3(vertex.x, vertex.y, vertex.z);
}

// See https://stackoverflow.com/a/23976134/17136841.
vec3 rayPlaneIntersection(vec3 rayOrigin, vec3 rayDirection, Plane plane) {
	float d = dot(plane.normal, rayDirection);

	if (abs(d) > 0.0001) {
		float t = dot(plane.offset - rayOrigin, plane.normal) / d;

		if (t >= 0.0001) {
			return rayOrigin + rayDirection * t;
		}
	}

	return rayOrigin;
}

void main() {
	const uint triangleCount = 4;
	const uint pointLightIndex = 0;

	uint casterTriangleIndex = gl_GlobalInvocationID.x;

	if (casterTriangleIndex >= triangleCount) {
		return;
	}

	vec3 p = pointLights[pointLightIndex].position;

	uint casterIndexOffset = casterTriangleIndex * 3;
	uint casterIndexA = indices[casterIndexOffset + 0];
	uint casterIndexB = indices[casterIndexOffset + 1];
	uint casterIndexC = indices[casterIndexOffset + 2];

	vec3 aC = loadVertexPosition(casterIndexA);
	vec3 bC = loadVertexPosition(casterIndexB);
	vec3 cC = loadVertexPosition(casterIndexC);

	vec3 normalC = normalize(cross(bC - aC, cC - aC));

	// The caster can't have a shadow if it points towards the light.
	if (dot(normalC, LIGHT_NORMAL) < 0.0) {
		return;
	}

	vec3 paC = normalize(aC - p);
	vec3 pbC = normalize(bC - p);
	vec3 pcC = normalize(cC - p);

	uint receiverTriangleIndex = 0;

	while (receiverTriangleIndex < triangleCount) {
		if (receiverTriangleIndex == casterTriangleIndex) {
			receiverTriangleIndex += 1;

			continue;
		}

		uint receiverIndexOffset = receiverTriangleIndex * 3;
		uint receiverIndexA = indices[receiverIndexOffset + 0];
		uint receiverIndexB = indices[receiverIndexOffset + 1];
		uint receiverIndexC = indices[receiverIndexOffset + 2];

		vec3 aR = loadVertexPosition(receiverIndexA);
		vec3 bR = loadVertexPosition(receiverIndexB);
		vec3 cR = loadVertexPosition(receiverIndexC);

		Plane receiver;
		receiver.normal = normalize(cross(bR - aR, cR - aR));

		// The receiver can't have a shadow cast on it if it points away from the caster.
		if (dot(normalC, receiver.normal) > 0.0) {
			receiverTriangleIndex += 1;

			continue;
		}

		receiver.offset = (aR + bR + cR) / 3;

		vec3 a = rayPlaneIntersection(aC, paC, receiver);
		vec3 b = rayPlaneIntersection(bC, pbC, receiver);
		vec3 c = rayPlaneIntersection(cC, pcC, receiver);

		uint vertexIndex = atomicAdd(drawCommand.vertexCount, 3);

		giVertices[vertexIndex + 0].position = vec4(c, 1.0);
		giVertices[vertexIndex + 0].a = vec4(aC, 1.0);
		giVertices[vertexIndex + 0].b = vec4(bC, 1.0);
		giVertices[vertexIndex + 0].c = vec4(cC, 1.0);

		giVertices[vertexIndex + 1].position = vec4(b, 1.0);
		giVertices[vertexIndex + 1].a = vec4(aC, 1.0);
		giVertices[vertexIndex + 1].b = vec4(bC, 1.0);
		giVertices[vertexIndex + 1].c = vec4(cC, 1.0);

		giVertices[vertexIndex + 2].position = vec4(a, 1.0);
		giVertices[vertexIndex + 2].a = vec4(aC, 1.0);
		giVertices[vertexIndex + 2].b = vec4(bC, 1.0);
		giVertices[vertexIndex + 2].c = vec4(cC, 1.0);

		receiverTriangleIndex += 1;
	}
}