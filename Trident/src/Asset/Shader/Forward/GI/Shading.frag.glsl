#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

in Interstage {
	layout(location = 0) vec3 surface;
	layout(location = 1) vec3 a;
	layout(location = 2) vec3 b;
	layout(location = 3) vec3 c;
} interstage;

const float PI = radians(180.0);

layout(location = 0) out vec4 fragment;

// Returns the angle formed by BA and BC.
float calculateAngle(vec3 a, vec3 b, vec3 c) {
	float numerator = (b.x - a.x) * (c.x - a.x) + (b.y - a.y) * (c.y - a.y) + (b.z - a.z) * (c.z - a.z);
	float denominator = sqrt(pow(b.x - a.x, 2.0) + pow(b.y - a.y, 2.0) + pow(b.z - a.z, 2.0)) * sqrt(pow(c.x - a.x, 2.0) + pow(c.y - a.y, 2.0) + pow(c.z - a.z, 2.0));
	float angle = acos(numerator / denominator) * (180.0 / PI);

	return angle;
}

void main() {
	vec3 p = interstage.surface;
	vec3 a = normalize(interstage.a - p);
	vec3 b = normalize(interstage.b - p);
	vec3 c = normalize(interstage.c - p);

	float angle = 2.0 * atan(length(dot(a, cross(b, c))), 1.0 + dot(a, b) + dot(b, c) + dot(c, a));

	vec3 L = vec3(angle);

	// fragment.rgb = (L / (PI * 2.0);
	fragment.rgb = vec3(0.0, 0.0, 0.0);
	fragment.a = angle / (PI * 2.0);
}