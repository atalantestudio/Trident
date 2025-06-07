#version 450

layout(location = 0) in vec2 vertex;

out VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

void main() {
	gl_Position = vec4(vertex, 0.0, 1.0);

	vertexOutput.textureCoordinate = vertex * 0.5 + 0.5;
}