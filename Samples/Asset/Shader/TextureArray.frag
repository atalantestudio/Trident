#version 450

in VertexOutput {
	layout(location = 0) vec2 textureCoordinate;
	layout(location = 1) flat uint textureIndex;
} vertexOutput;

layout(set = 0, binding = 0) uniform sampler2D samplerArray[4];

layout(location = 0) out vec4 outputTexel;

void main() {
	switch (vertexOutput.textureIndex) {
		case 0:
			outputTexel = texture(samplerArray[0], vertexOutput.textureCoordinate);

			break;
		case 1:
			outputTexel = texture(samplerArray[1], vertexOutput.textureCoordinate);

			break;
		case 2:
			outputTexel = texture(samplerArray[2], vertexOutput.textureCoordinate);

			break;
		case 3:
			outputTexel = texture(samplerArray[3], vertexOutput.textureCoordinate);

			break;
	}
}