#version 450

in VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
	layout(location = 1) flat uint textureIndex;
} vertexOutput;

layout(set = 0, binding = 0) uniform sampler2DArray textureArraySampler;

layout(location = 0) out vec4 color;

void main() {
	vec3 textureCoordinate = vec3(vertexOutput.textureCoordinate, float(vertexOutput.textureIndex));

	color = mix(vec4(textureCoordinate.xy, textureCoordinate.z / 4.0, 1.0), texture(textureArraySampler, textureCoordinate), 0.5);
}