#version 450

const vec4 CENTER_OF_MASS_COLOR = vec4(0.0, 0.0, 0.0, 1.0);
const float CENTER_OF_MASS_BORDER_SIZE = 0.05;

layout(location = 0) out vec4 color;

void main() {
	const float _CENTER_OF_MASS_BORDER_SIZE = pow(0.5 - CENTER_OF_MASS_BORDER_SIZE, 2.0);

	vec2 position = gl_PointCoord - 0.5;

	if (dot(position, position) > 0.25) {
		discard;
	}

	if (abs(dot(position, position)) < _CENTER_OF_MASS_BORDER_SIZE) {
		if (
			(position.x > 0.0 && position.y < 0.0) ||
			(position.x < 0.0 && position.y > 0.0)
		) {
			discard;
		}
	}

	color = CENTER_OF_MASS_COLOR;
}