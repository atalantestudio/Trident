#version 450

const vec4 EDGE_COLOR = vec4(1.0, 0.15, 0.0, 1.0);

layout(location = 0) out vec4 color;

void main() {
	color = EDGE_COLOR;
}