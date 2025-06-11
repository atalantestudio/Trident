#version 450

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 vertexOffset;
layout(location = 2) in uint textureIndex;

out VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
	layout(location = 1) flat uint textureIndex;
} vertexOutput;

void main() {
	gl_Position = vec4(vertex + vertexOffset, 0.0, 1.0);

	vertexOutput.textureCoordinate = vertex + 0.5;
	vertexOutput.textureCoordinate.y = 1.0 - vertexOutput.textureCoordinate.y;
	vertexOutput.textureIndex = textureIndex;
}