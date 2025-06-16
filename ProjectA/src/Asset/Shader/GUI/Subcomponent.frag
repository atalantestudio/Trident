#version 440

// If enabled, a vignette effect is applied to images with a radius > 0.
// If disabled, the image radius is treated as a border radius.
// #define VIGNETTE_EFFECT

layout(origin_upper_left) in vec4 gl_FragCoord;

in VertexOutput {
	layout(location = 0) flat vec4 bounds;
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
	vec2 minBounds = vertexOutput.bounds.xy;
	vec2 maxBounds = vertexOutput.bounds.zw;
	float r = vertexOutput.radius;

	// Screen space coordinate of the current fragment.
	vec2 p = floor(gl_FragCoord.xy) + 0.5;

	// Closest point of p on the inner rectangle.
	vec2 a = clamp(p, minBounds, maxBounds);

	// [0, pow(r * sqrt(2), 2)]
	float d = lengthSquared(p - a);

#ifndef VIGNETTE_EFFECT
	if (d > r * r) {
		discard;
	}

	color = texture(textureArraySampler, vertexOutput.textureCoordinate) * vertexOutput.color;
#else
	float m = 1.0;

	if (r > 0.0) {
		m -= d / pow(r * sqrt(2.0), 2.0);
	}

	color = texture(textureArraySampler, vertexOutput.textureCoordinate) * vertexOutput.color * m;
#endif
}