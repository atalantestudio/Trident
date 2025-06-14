#version 440

// If enabled, a vignette effect is applied to images with a radius > 0.
// If disabled, the image radius is treated as a border radius.
// #define VIGNETTE_EFFECT

in VertexOutput {
	layout(location = 0) flat vec4 centerAndHalfInnerSize;
	layout(location = 1) flat vec4 color;
	layout(location = 2) flat float radius;
	layout(location = 3) vec3 textureCoordinate;
} vertexOutput;

layout(set = 0, binding = 1) uniform sampler2DArray textureArraySampler;

layout(location = 0) out vec4 color;

float lengthSquared(vec2 a) {
	return dot(a, a);
}

void main() {
	// Screen space coordinate of the current fragment.
	vec2 p = gl_FragCoord.xy;

	vec2 center = vertexOutput.centerAndHalfInnerSize.xy;
	vec2 halfInnerSize = vertexOutput.centerAndHalfInnerSize.zw;
	float m = 1.0;

	// Closest point of p on the inner rectangle.
	// TODO: Use min/max bounds instead of center/half inner size?
	vec2 a = clamp(p, center - halfInnerSize, center + halfInnerSize);

	// d E [0, pow(R * sqrt(2), 2)]
	float d = abs(lengthSquared(p - a));

#ifndef VIGNETTE_EFFECT
	if (d > vertexOutput.radius * vertexOutput.radius) {
		discard;
	}
#else
	if (vertexOutput.radius > 0.0) {
		m -= d / pow(vertexOutput.radius * sqrt(2.0), 2.0);
	}
#endif

	color = texture(textureArraySampler, vertexOutput.textureCoordinate) * vertexOutput.color * m;
}