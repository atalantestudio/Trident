#version 450

in VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

layout(set = 0, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) out vec4 color;

void main() {
	vec2 textureCoordinate = vertexOutput.textureCoordinate;

	textureCoordinate.y = 1.0 - textureCoordinate.y;

	color = texture(textureSampler, textureCoordinate);
}