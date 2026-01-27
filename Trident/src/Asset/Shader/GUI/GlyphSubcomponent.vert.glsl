#version 440

layout(location = 0) in vec4 vertexAndTextureCoordinate;
layout(location = 1) in mat3x3 worldTransform;
layout(location = 4) in mat3x3 textureTransform;
layout(location = 7) in uint layerIndex;
layout(location = 8) in vec4 color;
layout(location = 9) in float fontSize;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
} camera;

out VertexOutput {
	layout(location = 0) vec3 textureCoordinate;
	layout(location = 1) flat vec4 color;
	layout(location = 2) flat float fontSize;
} vertexOutput;

void main() {
	vec3 vertex = vec3(vertexAndTextureCoordinate.xy, 1.0);
	vec3 textureCoordinate = vec3(vertexAndTextureCoordinate.zw, 1.0);

	gl_Position = vec4(camera.projection * worldTransform * vertex, 1.0);

	vertexOutput.textureCoordinate = textureTransform * textureCoordinate;
	vertexOutput.textureCoordinate.z = float(layerIndex);
	vertexOutput.color = color;
	vertexOutput.fontSize = fontSize;
}