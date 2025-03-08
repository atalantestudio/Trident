#version 310 es

precision mediump float;

layout(location = 0) flat in uint v_colorIndex;
layout(location = 1) flat in uint v_textureIndex;

layout(location = 0) out vec4 color;

layout(set = 0, binding = 1) buffer ColorBuffer {
	vec4 colors[];
};

void main() {
	color = colors[v_colorIndex];
}