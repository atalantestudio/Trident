#version 440

const float ALPHA_THRESHOLD = 0.5;
const float SMOOTHNESS = 0.05;

in VertexOutput {
	layout(location = 0) vec3 textureCoordinate;
	layout(location = 1) vec4 color;
} vertexOutput;

layout(set = 0, binding = 1) uniform sampler2DArray textureArraySampler;

layout(location = 0) out vec4 color;

void main() {
	float value = texture(textureArraySampler, vertexOutput.textureCoordinate).r;
	/* float value = smoothstep(
		ALPHA_THRESHOLD - SMOOTHNESS,
		ALPHA_THRESHOLD + SMOOTHNESS,
		texture(textureArraySampler, vertexOutput.textureCoordinate).r
	); */

	color = vertexOutput.color * value;

	/* const vec2 dropShadowOffset = vec2(0.0, 0.001);

	float offsetDistance = texture(glyphMapSampler, vertexOutput.imageCoordinate.xy - dropShadowOffset).r;

	float dropShadow = smoothstep(0.0, 1.0, offsetDistance);

	const vec4 dropShadowColor = vec4(vec3(0.4), 1.0);

	color = mix(dropShadowColor * dropShadow, color, color.a); */
}