#version 440

precision mediump float;

in VertexOutput {
	layout(location = 0) vec4 color;
} vertexOutput;

//layout(set = 0, binding = 1) uniform sampler2D samplers;

/*layout(set = 0, binding = 2) buffer ColorBuffer {
	vec4 colors[];
};*/

layout(location = 0) out vec4 color;

void main() {
	color = vertexOutput.color;
}