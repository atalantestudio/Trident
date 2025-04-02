#version 440

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexTextureCoordinates;
layout(location = 2) in mat3x3 world;
layout(location = 3) in vec2 textureCoordinates;
layout(location = 7) in uint colorIndex;
layout(location = 8) in float textureIndex;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
} camera;

out VertexOutput {
	layout(location = 0) vec3 textureCoordinates;
	//layout(location = 1) flat uint colorIndex;
} vertexOutput;

void main() {
	gl_Position = vec4(camera.projection * world * vertexPosition, 1.0);

	vertexOutput.textureCoordinates = vec3(vertexTextureCoordinates, textureIndex);
	//vertexOutput.colorIndex = colorIndex;
}