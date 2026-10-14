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
	vec3 position;
	uint _;
};

struct PointLight {
	vec3 color;
	float intensity;
	vec3 position;
};

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

vec3 loadVertexPosition(uint index) {
	Vertex vertex = vertices[index];

	return vec3(vertex.x, vertex.y, vertex.z);
}

// See https://stackoverflow.com/a/23976134/17136841.
bool rayPlaneIntersection(vec3 rayOrigin, vec3 rayDirection, vec3 planeNormal, vec3 planeCenter, inout vec3 i) {
	float d = dot(planeNormal, rayDirection);

	if (abs(d) > 0.0001) {
		float t = dot(planeCenter - rayOrigin, planeNormal) / d;

		if (t >= 0.0001) {
			i = rayOrigin + rayDirection * t;

			return true;
		}
	}

	return false;
}

void main() {
	const uint CASTER_INDEX_OFFSET = 6;
	const uint RECEIVER_INDEX_OFFSET = 0;

	uint pointLightIndex = 0;
	uint triangleIndex = gl_GlobalInvocationID.x;
	uint indexOffset = triangleIndex * 3;

	vec3 p = pointLights[pointLightIndex].position;

	uint casterIndexOffset = CASTER_INDEX_OFFSET + indexOffset;
	uint casterIndexA = indices[casterIndexOffset + 0];
	uint casterIndexB = indices[casterIndexOffset + 1];
	uint casterIndexC = indices[casterIndexOffset + 2];

	vec3 a = loadVertexPosition(casterIndexA);
	vec3 b = loadVertexPosition(casterIndexB);
	vec3 c = loadVertexPosition(casterIndexC);

	if (dot(b - a, p - a) < 0.0) {
		giVertices[indexOffset + 0].position = vec3(0.0, 0.0, 0.0);
		giVertices[indexOffset + 1].position = vec3(0.0, 0.0, 0.0);
		giVertices[indexOffset + 2].position = vec3(0.0, 0.0, 0.0);

		return;
	}

	uint receiverIndexOffset = RECEIVER_INDEX_OFFSET;// + indexOffset;
	uint receiverIndexA = indices[receiverIndexOffset + 0];
	uint receiverIndexB = indices[receiverIndexOffset + 1];
	uint receiverIndexC = indices[receiverIndexOffset + 2];

	vec3 d = loadVertexPosition(receiverIndexA);
	vec3 e = loadVertexPosition(receiverIndexB);
	vec3 f = loadVertexPosition(receiverIndexC);

	// vec3 planeNormal = normalize(cross(normalize(e - d), normalize(f - d)));
	vec3 planeNormal = vec3(0.0, 1.0, 0.0);
	vec3 planeCenter = vec3(0.0, 0.0, 0.0);

	vec3 pa = normalize(a - p);
	vec3 pb = normalize(b - p);
	vec3 pc = normalize(c - p);

	vec3 a2 = a;
	vec3 b2 = b;
	vec3 c2 = c;

	rayPlaneIntersection(a, pa, planeNormal, planeCenter, a2);
	rayPlaneIntersection(b, pb, planeNormal, planeCenter, b2);
	rayPlaneIntersection(c, pc, planeNormal, planeCenter, c2);

	giVertices[indexOffset + 0].position = c2;
	giVertices[indexOffset + 1].position = b2;
	giVertices[indexOffset + 2].position = a2;
}