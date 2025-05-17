#version 440

precision mediump float;

in VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

layout(set = 0, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) out vec4 color;

void main() {
	color = texture(textureSampler, vertexOutput.textureCoordinate);
}