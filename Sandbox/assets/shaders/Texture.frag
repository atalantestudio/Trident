#version 450

in VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

layout(set = 0, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) out vec4 outputTexel;

void main() {
	vec2 textureCoordinate = vec2(
		vertexOutput.textureCoordinate.x,
		1.0 - vertexOutput.textureCoordinate.y
	);

	float color = texture(textureSampler, textureCoordinate).r;

	outputTexel.rgb = vec3(color);
}