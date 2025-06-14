#version 440

layout(location = 0) in vec4 vertexAndTextureCoordinate;
layout(location = 1) in vec4 centerAndHalfInnerSize;
layout(location = 2) in mat3x3 worldTransform;
layout(location = 5) in mat3x3 textureTransform;
layout(location = 8) in uint layerIndex;
layout(location = 9) in vec4 color;
layout(location = 10) in float radius;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
} camera;

out VertexOutput {
	layout(location = 0) flat vec4 centerAndHalfInnerSize;
	layout(location = 1) flat vec4 color;
	layout(location = 2) flat float radius;
	layout(location = 3) vec3 textureCoordinate;
} vertexOutput;

void main() {
	vec3 vertex = vec3(vertexAndTextureCoordinate.xy, 1.0);
	vec3 textureCoordinate = vec3(vertexAndTextureCoordinate.zw, 1.0);

	gl_Position = vec4(camera.projection * worldTransform * vertex, 1.0);

	vertexOutput.centerAndHalfInnerSize = centerAndHalfInnerSize;
	vertexOutput.color = color;
	vertexOutput.radius = radius;
	vertexOutput.textureCoordinate = textureTransform * textureCoordinate;
	vertexOutput.textureCoordinate.z = float(layerIndex);
}