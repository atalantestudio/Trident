#version 450

layout(location = 0) in vec2 textureCoordinate;

layout(set = 0, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) out vec4 color;

const float ALPHA_THRESHOLD = 0.5;

void main() {
	vec2 uv = vec2(textureCoordinate.x, 1.0 - textureCoordinate.y);
	vec4 texel = texture(textureSampler, uv);

	//color.rgb = vec3(texel.r >= ALPHA_THRESHOLD);
	color.rgb = vec3(texel.r);
}