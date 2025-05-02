#version 440

precision mediump float;

/*in VertexOutput {
	layout(location = 0) vec3 textureCoordinates;
	layout(location = 1) flat uint colorIndex;
} vertexOutput;*/

//layout(set = 0, binding = 1) uniform sampler2D samplers;

/*layout(set = 0, binding = 2) buffer ColorBuffer {
	vec4 colors[];
};*/

layout(location = 0) out vec4 color;

void main() {
	//color = colors[vertexOutput.colorIndex];
	//color = texture(samplers, vertexOutput.textureCoordinates);
	//color = texture(samplers, vertexOutput.textureCoordinates.xy) + vec4(1, 0, 0.2, 1);
	color = vec4(1.0, 0.2, 0.0, 1.0);
}