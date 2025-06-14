#version 440

const float ALPHA_THRESHOLD = 0.5;
const float SMOOTHNESS = 0.05;

in VertexOutput {
	layout(location = 0) vec3 textureCoordinate;
	layout(location = 1) float outlineWidth;
	layout(location = 2) vec4 color;
	layout(location = 3) vec4 outlineColor;
} vertexOutput;

layout(set = 0, binding = 1) uniform sampler2DArray textureArraySampler;

layout(location = 0) out vec4 color;

void main() {
	float value = texture(textureArraySampler, vertexOutput.textureCoordinate).r;
	/*float value = smoothstep(
		ALPHA_THRESHOLD - SMOOTHNESS,
		ALPHA_THRESHOLD + SMOOTHNESS,
		texture(textureArraySampler, vertexOutput.textureCoordinate).r
	);*/

	color = vertexOutput.color * value;

	/* const float outlineMin0 = 0.05;
	const float outlineMin1 = 0.1;
	const float outlineMax0 = 0.95;
	const float outlineMax1 = 1.0;

	if (value > outlineMin0 && value <= outlineMax1) {
		float outlineFactor = 1.0;

		if (value <= outlineMin1) {
			outlineFactor = smoothstep(outlineMin0, outlineMin1, value);
		}
		else {
			outlineFactor = smoothstep(outlineMax0, outlineMax1, value);
		}

		color = mix(vertexOutput.outlineColor, color, outlineFactor);
	}

	const vec2 dropShadowOffset = vec2(0.0, 0.001);

	float offsetDistance = texture(glyphMapSampler, vertexOutput.imageCoordinate.xy - dropShadowOffset).r;

	float dropShadow = smoothstep(0.0, 1.0, offsetDistance);

	const vec4 dropShadowColor = vec4(vec3(0.4), 1.0);

	color = mix(dropShadowColor * dropShadow, color, color.a); */
}