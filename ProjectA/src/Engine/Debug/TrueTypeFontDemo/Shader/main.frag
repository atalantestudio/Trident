#version 450

layout(location = 0) in vec2 textureCoordinate;

layout(set = 0, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) out vec4 color;

void main() {
	vec2 uv = vec2(textureCoordinate.x, 1.0 - textureCoordinate.y);
	float textureSample = texture(textureSampler, uv).r;

	color = vec4(vec3(textureSample), 1.0);
}