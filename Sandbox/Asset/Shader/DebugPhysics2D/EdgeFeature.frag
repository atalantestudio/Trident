#version 450

layout(location = 0) out vec4 color;

const vec4 EDGE_COLOR = vec4(1.0, 0.15, 0.0, 1.0);

void main() {
	color = EDGE_COLOR;
}