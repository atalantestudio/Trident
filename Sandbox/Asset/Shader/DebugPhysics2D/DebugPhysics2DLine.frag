#version 450

layout(set = 0, binding = 0) uniform Line {
	vec4 color;
} line;

layout(location = 0) out vec4 color;

void main() {
	color = line.color;
}