#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

in Interstage {
	layout(location = 0) flat uint triangleIndex;
	layout(location = 1) vec3 position;
	layout(location = 2) vec3 vertexNormal;
	layout(location = 3) flat vec3 triangleNormal;
} interstage;

layout(location = 0) out vec4 position;
layout(location = 1) out vec4 vertexNormal;
layout(location = 2) out vec4 triangleNormal;

void main() {
	position = vec4(interstage.position, interstage.triangleIndex + 1u);
	vertexNormal = vec4(interstage.vertexNormal, 0.0);
	triangleNormal = vec4(interstage.triangleNormal, 0.0);
}