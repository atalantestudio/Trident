#version 450

in VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
	layout(location = 1) flat uint textureIndex;
} vertexOutput;

layout(binding = 0, set = 0) uniform sampler2D samplerArray[4];

layout(location = 0) out vec4 outputTexel;

void main() {
	vec4 texel = texture(samplerArray[vertexOutput.textureIndex], vertexOutput.textureCoordinate);

	outputTexel = texel;
}