#version 320 es

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in vec4 vertexAndTextureCoordinate;
layout(location = 1) in mat3x3 worldTransform;
layout(location = 4) in mat3x3 textureTransform;
layout(location = 7) in vec4 color;
layout(location = 8) in uint zIndex;
layout(location = 9) in uint textureArrayLayerIndex;
layout(location = 10) in float fontSize;

// TODO(gui/text-smoothness)
// layout(location = 11) in float a;
// layout(location = 12) in float b;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat3x3 projection;
} camera;

out Interstage {
	layout(location = 0) flat vec4 color;
	layout(location = 1) vec3 textureCoordinate;
	layout(location = 2) flat float fontSize;

	// TODO(gui/text-smoothness)
	// layout(location = 3) float a;
	// layout(location = 4) float b;
} interstage;

void main() {
	vec3 vertex = vec3(vertexAndTextureCoordinate.xy, 1.0);
	vec3 textureCoordinate = vec3(vertexAndTextureCoordinate.zw, 1.0);

	gl_Position = vec4(camera.projection * worldTransform * vertex, 1.0);
	gl_Position.z = float(zIndex) / 100.0;

	interstage.color = color;
	interstage.textureCoordinate = textureTransform * textureCoordinate;
	interstage.textureCoordinate.z = float(textureArrayLayerIndex);
	interstage.fontSize = fontSize;

	// TODO(gui/text-smoothness)
	// interstage.a = a;
	// interstage.b = b;
}