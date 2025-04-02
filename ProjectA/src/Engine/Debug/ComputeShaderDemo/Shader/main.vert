#version 450

layout(location = 0) in vec2 vertexPosition;

layout(location = 0) out vec2 textureCoordinate;

void main() {
	gl_Position = vec4(vertexPosition, 0.0, 1.0);

	textureCoordinate = vertexPosition * 0.5 + 0.5;
}