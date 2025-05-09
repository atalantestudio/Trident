#version 450

layout(location = 0) in vec2 vertexPosition;

out VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

void main() {
	gl_Position = vec4(vertexPosition, 0.0, 1.0);

	vertexOutput.textureCoordinate = vertexPosition * 0.5;
}