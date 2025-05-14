#version 450

layout(location = 0) out vec4 color;

const vec4 VERTEX_COLOR = vec4(1.0, 0.15, 0.0, 1.0);

void main() {
	vec2 position = gl_PointCoord - 0.5;

	if (dot(position, position) > 0.25) {
		discard;
	}

	color = VERTEX_COLOR;
}