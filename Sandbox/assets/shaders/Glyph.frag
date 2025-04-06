#version 450

in VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

layout(set = 0, binding = 1) uniform sampler2D textureSampler;

layout(location = 0) out vec4 color;

const float ALPHA_THRESHOLD = 0.5;

void main() {
	vec2 textureCoordinate = vertexOutput.textureCoordinate;

	vec4 texel = texture(textureSampler, textureCoordinate);

	color.rgb = vec3(texel.r >= ALPHA_THRESHOLD);
	//color.rgb += vec3(1.0, 0.2, 0.0);
}