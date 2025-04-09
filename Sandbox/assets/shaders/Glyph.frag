#version 450

in VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

layout(set = 0, binding = 1) uniform sampler2D textureSampler;

layout(location = 0) out vec4 color;

const float ALPHA_THRESHOLD = 0.5;
const float SMOOTHNESS = 0.05;

void main() {
	vec2 textureCoordinate = vertexOutput.textureCoordinate;

	float value = smoothstep(
		ALPHA_THRESHOLD - SMOOTHNESS,
		ALPHA_THRESHOLD + SMOOTHNESS,
		texture(textureSampler, textureCoordinate).r
	);
	//float value = texture(textureSampler, textureCoordinate).r;

	color.rgb = vec3(value);
	color.rgb += vec3(78.0, 115.0, 150.0) / 255.0;
}