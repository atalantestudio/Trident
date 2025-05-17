// This shader is only not meant to be compiled to SPIR-V.

#version 440

const vec2 VERTICES[] = vec2[](
	vec2(-1.0,  1.0),
	vec2( 3.0,  1.0),
	vec2(-1.0, -3.0)
);

out VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

void main() {
	vec2 vertex = VERTICES[gl_VertexID];

	gl_Position = vec4(vertex, 0.0, 1.0);

	vertexOutput.textureCoordinate = vertex * 0.5 + 0.5;
}