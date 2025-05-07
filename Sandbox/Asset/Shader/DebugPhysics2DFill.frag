#version 450

in VertexOutput {
	layout(location = 0) vec4 fillColor;
} vertexOutput;

layout(location = 0) out vec4 color;

void main() {
	color = vertexOutput.fillColor;
}