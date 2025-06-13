#version 440

in VertexOutput {
	layout(location = 0) vec3 textureCoordinate;
	layout(location = 1) vec4 color;
} vertexOutput;

layout(set = 0, binding = 1) uniform sampler2DArray textureArraySampler;

layout(location = 0) out vec4 color;

void main() {
	color = texture(textureArraySampler, vertexOutput.textureCoordinate) * vertexOutput.color;
}