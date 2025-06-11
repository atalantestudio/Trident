#version 450

in VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
	layout(location = 1) flat uint textureIndex;
} vertexOutput;

layout(set = 0, binding = 0) uniform sampler2D textureSamplers[4];

layout(location = 0) out vec4 color;

void main() {
	switch (vertexOutput.textureIndex) {
		case 0:
			color = texture(textureSamplers[0], vertexOutput.textureCoordinate);

			break;
		case 1:
			color = texture(textureSamplers[1], vertexOutput.textureCoordinate);

			break;
		case 2:
			color = texture(textureSamplers[2], vertexOutput.textureCoordinate);

			break;
		case 3:
			color = texture(textureSamplers[3], vertexOutput.textureCoordinate);

			break;
	}
}