#version 440

// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

float lengthSquared(vec2 a) {
	return dot(a, a);
}

// delta = absolute difference between the current fragment and the center of the rectangle
// halfSize = size of the rectangle divided by 2
// r = border radius
float RoundedBoxSignedDistance(vec2 delta, vec2 halfSize, float r) {
	vec2 d = delta - halfSize + r;

	// return length(max(d, 0.0)) - r;
	return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - r;
}

layout(origin_upper_left) in;

in FragmentInput {
	layout(location = 0) flat vec4 bounds;
	layout(location = 1) flat vec4 color;
	layout(location = 2) flat float outlineSize;
	layout(location = 3) flat vec4 outlineColor;
	layout(location = 4) flat float borderRadius;
	layout(location = 5) vec3 textureCoordinate;
} fragmentInput;

layout(set = 0, binding = 1) uniform sampler2DArray textureArraySampler;

layout(location = 0) out vec4 fragment;

void main() {
	vec2 minBounds = fragmentInput.bounds.xy;
	vec2 maxBounds = fragmentInput.bounds.zw;
	float r = fragmentInput.borderRadius;
	float outlineSize = fragmentInput.outlineSize;

	// Screen-space coordinate of the current fragment.
	// vec2 p = floor(gl_FragCoord.xy) + 0.5;
	vec2 p = gl_FragCoord.xy;

	vec2 size = maxBounds - minBounds;
	vec2 center = mix(minBounds, maxBounds, 0.5);

	float d = RoundedBoxSignedDistance(abs(p - center), size * 0.5, r);
	float fw = fwidth(d);
	float content = smoothstep(-fw, fw, -d - outlineSize);

	// TODO: Calculate the correct outline inner radius.
	// TODO: Restore content.
	fragment = fragmentInput.outlineColor * content * outlineSize + fragmentInput.color * texture(textureArraySampler, fragmentInput.textureCoordinate); // * content;
}