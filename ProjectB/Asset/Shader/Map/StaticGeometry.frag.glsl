#version 320 es

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

precision highp float;

const float PI = radians(180.0);

const uint CARVING_COUNT = 2u;

const uint CARVING_TRIANGLE_COUNTS[] = uint[](
	// Hall entrance
	2u,

	// Main entrance
	2u
);

const uint CARVING_INDICES[] = uint[](
	// Hall entrance
	0u, 1u, 2u,
	1u, 3u, 2u,

	// Main entrance
	4u, 5u, 6u,
	5u, 7u, 6u
);

const vec3 CARVING_VERTICES[] = vec3[](
	// Hall entrance
	vec3(92.0,  112.0, -16.0),
	vec3(36.0,  112.0, -16.0),
	vec3(36.0,  4.0,   -16.0),
	vec3(92.0,  4.0,   -16.0),

	// Main entrance
	vec3(24.0,  112.0,  184.0),
	vec3(84.0,  112.0,  184.0),
	vec3(84.0,  4.0,    184.0),
	vec3(24.0,  4.0,    184.0)
);

in Interstage {
	layout(location = 0) vec3 surface;
	layout(location = 1) vec3 normal;
} interstage;

layout(set = 0, binding = 0) uniform readonly Camera {
	mat4x4 projectionView;
	vec3 position;
} camera;

layout(location = 0) out vec4 fragment;

float triangleSolidAngle(vec3 a, vec3 b, vec3 c) {
	return 2.0 * atan(length(dot(a, cross(b, c))), 1.0 + dot(a, b) + dot(b, c) + dot(c, a));
}

vec3 tonemap(vec3 rgb) {
	return 1.0 - exp(rgb * -1.2);
}

vec3 gammaCorrect(vec3 rgb) {
	const vec3 exponent = vec3(1.0, 1.0, 1.0) / 2.2;

	return pow(rgb, exponent);
}

void main() {
	/*
	for each opaque triangle of the room {
		add SA * triangle color
	}

	for each connector {
		if connector leads to nowhere {
			add SA * average sky color towards surface normal

			continue
		}

		create a copy of room2's vertices

		for each vertex {
			clip vertex against the connector's edges
		}

		for each opaque triangle of room2 {
			calculate SA from combination of clipped vertices and connector vertices
			add SA * triangle color
		}

		for each connector of room2 (not connecting with room1) {
			repeat
		}
	}
	*/

	// TODO(project-b/map-renderer/gi)
	/* uint carvingIndex = 0u;
	uint triangleOffset = 0u;
	float L0 = 0.0;

	while (carvingIndex < CARVING_COUNT) {
		uint triangleCount = CARVING_TRIANGLE_COUNTS[carvingIndex];
		uint triangleIndex = 0u;

		while (triangleIndex < triangleCount) {
			vec3 a = normalize(CARVING_VERTICES[CARVING_INDICES[triangleOffset * 3u + 0u]] - interstage.surface);
			vec3 b = normalize(CARVING_VERTICES[CARVING_INDICES[triangleOffset * 3u + 1u]] - interstage.surface);
			vec3 c = normalize(CARVING_VERTICES[CARVING_INDICES[triangleOffset * 3u + 2u]] - interstage.surface);

			L0 += triangleSolidAngle(a, b, c);

			triangleIndex += 1u;
			triangleOffset += 1u;
		}

		carvingIndex += 1u;
	}

	float L1 = PI * 2.0 - L0;
	vec3 L = L0 * vec3(0.6, 0.7, 0.9) + L1 * vec3(0.0, 0.0, 0.0);

	fragment.rgb = L / (PI * 2.0);
	fragment.rgb = tonemap(fragment.rgb);
	fragment.rgb = gammaCorrect(fragment.rgb); */

	vec3 surfaceToCamera = normalize(camera.position - interstage.surface);
	float angle = dot(interstage.normal, surfaceToCamera);

	fragment.rgb = vec3(angle);
	fragment.a = 1.0;
}