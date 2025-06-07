#version 450

in VertexOutput {
	layout(location = 0) vec3 color;
} vertexOutput;

layout(location = 0) out vec4 color;

void main() {
	color = vec4(vertexOutput.color, 1.0);
}