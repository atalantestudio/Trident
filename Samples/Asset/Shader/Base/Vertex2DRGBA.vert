#version 450

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec4 color;

out VertexOutput {
	layout(location = 0) vec4 color;
} vertexOutput;

void main() {
	gl_Position = vec4(vertex, 0.0, 1.0);

	vertexOutput.color = color;
}