#version 440

// const vec2 DROP_SHADOW_OFFSET = vec2(0.0, 0.001);
// const vec4 DROP_SHADOW_COLOR = vec4(vec3(0.4), 1.0);

const float ALPHA_THRESHOLD = 0.5;
const float SMOOTHNESS = 0.25;

in VertexOutput {
	layout(location = 0) vec3 textureCoordinate;
	layout(location = 1) flat vec4 color;
} vertexOutput;

layout(set = 0, binding = 1) uniform sampler2DArray textureArraySampler;

layout(location = 0) out vec4 fragment;

void main() {
	float value = smoothstep(
		ALPHA_THRESHOLD - SMOOTHNESS,
		ALPHA_THRESHOLD + SMOOTHNESS,
		texture(textureArraySampler, vertexOutput.textureCoordinate).r
	);

	fragment = vertexOutput.color * value;

	/* float offsetDistance = texture(glyphMapSampler, vertexOutput.imageCoordinate.xy - DROP_SHADOW_OFFSET).r;
	float dropShadow = smoothstep(0.0, 1.0, offsetDistance);

	fragment = mix(DROP_SHADOW_COLOR * dropShadow, fragment, fragment.a); */
}