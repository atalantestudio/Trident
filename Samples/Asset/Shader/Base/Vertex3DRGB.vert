#version 450

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 color;

out VertexOutput {
	layout(location = 0) vec3 color;
} vertexOutput;

void main() {
	gl_Position = vec4(vertex, 1.0);

	vertexOutput.color = color;
}