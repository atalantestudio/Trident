#version 450

in VertexOutput {
	layout(location = 0) vec4 color;
} vertexOutput;

layout(location = 0) out vec4 color;

void main() {
	color = vertexOutput.color;
}