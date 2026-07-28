#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

in Interstage {
	layout(location = 0) vec3 position;
	layout(location = 1) flat uint triangleIndex;
	layout(location = 2) flat vec3 triangleNormal;
	layout(location = 3) vec3 vertexNormal;
} interstage;

layout(location = 0) out vec4 position;
layout(location = 1) out vec4 triangleNormal;
layout(location = 2) out vec4 vertexNormal;

void main() {
	vec3 luxelSize = max(abs(dFdx(interstage.position)), abs(dFdy(interstage.position)));
	float diagonalLuxelSize = max(max(luxelSize.x, luxelSize.y), luxelSize.z) * sqrt(2.0);

	position = vec4(interstage.position, interstage.triangleIndex);
	triangleNormal = vec4(normalize(interstage.triangleNormal), diagonalLuxelSize);
	vertexNormal = vec4(normalize(interstage.vertexNormal), 0.0);
}