#version 450

in VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

layout(set = 0, binding = 1) uniform sampler2D textureSampler;

layout(location = 0) out vec4 color;

void main() {
	color = texture(textureSampler, vertexOutput.textureCoordinate);
}