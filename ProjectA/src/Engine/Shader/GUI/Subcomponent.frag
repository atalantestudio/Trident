#version 320 es

precision mediump float;

in VertexOutput {
	layout(location = 0) flat uint colorIndex;
	layout(location = 1) flat uint textureIndex;
} vertexOutput;

layout(location = 0) out vec4 color;

layout(set = 0, binding = 1) buffer ColorBuffer {
	vec4 colors[];
};

void main() {
	color = colors[vertexOutput.colorIndex];
}