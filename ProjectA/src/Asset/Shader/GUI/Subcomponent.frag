#version 440

#define EFFECT_TYPE_BEVEL 0
#define EFFECT_TYPE_ROUND 1
#define EFFECT_TYPE_VIGNETTE 2

#define BORDER_EFFECT_TYPE EFFECT_TYPE_ROUND

#if BORDER_EFFECT_TYPE == EFFECT_TYPE_BEVEL
	bool beveledCorner(vec2 delta, float radius) {
		return abs(delta.x) + abs(delta.y) > radius;
	}
#elif BORDER_EFFECT_TYPE == EFFECT_TYPE_ROUND
	bool roundedCorner(vec2 delta, float radius) {
		return dot(delta, delta) > radius * radius;
	}
#endif

layout(origin_upper_left) in vec4 gl_FragCoord;

in VertexOutput {
	layout(location = 0) flat vec4 bounds;
	layout(location = 1) flat vec4 color;
	layout(location = 2) flat float radius;
	layout(location = 3) vec3 textureCoordinate;
} vertexOutput;

layout(set = 0, binding = 1) uniform sampler2DArray textureArraySampler;

layout(location = 0) out vec4 color;

void main() {
	vec2 minBounds = vertexOutput.bounds.xy;
	vec2 maxBounds = vertexOutput.bounds.zw;
	float r = vertexOutput.radius;

	// Screen space coordinate of the current fragment.
	vec2 p = floor(gl_FragCoord.xy) + 0.5;

	// Closest point of p on the inner rectangle.
	vec2 a = clamp(p, minBounds, maxBounds);

	vec2 ap = p - a;

#if BORDER_EFFECT_TYPE != EFFECT_TYPE_VIGNETTE
	bool condition;

	#if BORDER_EFFECT_TYPE == EFFECT_TYPE_BEVEL
		condition = beveledCorner(ap, r);
	#elif BORDER_EFFECT_TYPE == EFFECT_TYPE_ROUND
		condition = roundedCorner(ap, r);
	#endif

	if (condition) {
		discard;
	}

	color = texture(textureArraySampler, vertexOutput.textureCoordinate) * vertexOutput.color;
#else
	float d = dot(ap, ap);
	float m = 1.0;

	if (r > 0.0) {
		m -= d / pow(r * sqrt(2.0), 2.0);
	}

	color = texture(textureArraySampler, vertexOutput.textureCoordinate) * vertexOutput.color * m;
#endif
}