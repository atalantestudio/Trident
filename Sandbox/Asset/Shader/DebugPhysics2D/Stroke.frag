#version 450

const vec3 LINE_COLOR = vec3(0.0, 0.0, 0.0);
const vec3 SELECTED_LINE_COLOR = vec3(0.299, 0.922, 0.0);

in VertexOutput {
	layout(location = 0) flat uint selected;
} vertexOutput;

layout(location = 0) out vec4 color;

void main() {
	if (vertexOutput.selected == 1) {
		color.rgb = SELECTED_LINE_COLOR;
	}
	else {
		color.rgb = LINE_COLOR;
	}

	color.a = 1.0;
}