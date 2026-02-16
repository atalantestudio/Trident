#version 440

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(location = 0) in vec4 vertexAndTextureCoordinate;
// layout(location = 1) in vec4 bounds;
layout(location = 1) in mat3x3 worldTransform;
layout(location = 4) in mat3x3 textureTransform;
layout(location = 7) in vec4 color;
// layout(location = 9) in vec4 outlineColor;
layout(location = 8) in uint textureArrayLayerIndex;
// layout(location = 11) in float outlineSize;
// layout(location = 12) in float borderRadius;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
} camera;

out VertexOutput {
	layout(location = 0) flat vec4 color;
	layout(location = 1) vec3 textureCoordinate;
	// layout(location = 0) flat vec4 bounds;
	// layout(location = 2) flat float outlineSize;
	// layout(location = 3) flat vec4 outlineColor;
	// layout(location = 4) flat float borderRadius;
} vertexOutput;

void main() {
	vec3 vertex = vec3(vertexAndTextureCoordinate.xy, 1.0);
	vec3 textureCoordinate = vec3(vertexAndTextureCoordinate.zw, 1.0);

	gl_Position = vec4(camera.projection * worldTransform * vertex, 1.0);

	// vertexOutput.bounds = bounds;
	// vertexOutput.outlineSize = outlineSize;
	// vertexOutput.outlineColor = outlineColor;
	// vertexOutput.borderRadius = borderRadius;
	vertexOutput.color = color;
	vertexOutput.textureCoordinate = textureTransform * textureCoordinate;
	vertexOutput.textureCoordinate.z = float(textureArrayLayerIndex);
}