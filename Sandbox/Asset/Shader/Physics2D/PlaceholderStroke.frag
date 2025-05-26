#version 450

const float LINE_DASH_SIZE = 6.0;
const float LINE_GAP_SIZE = 5.0;
const vec3 LINE_DASH_COLOR = vec3(0.0, 0.0, 0.0);

in VertexOutput {
	layout(location = 0) flat vec2 startPosition;
	layout(location = 1) vec2 currentPosition;
} vertexOutput;

layout(set = 0, binding = 2) uniform View {
	vec2 viewport;
} view;

layout(location = 0) out vec4 color;

void main() {
	float offset = length((vertexOutput.currentPosition - vertexOutput.startPosition) * view.viewport * 0.5);

	if (fract(offset / (LINE_DASH_SIZE + LINE_GAP_SIZE)) > LINE_DASH_SIZE / (LINE_DASH_SIZE + LINE_GAP_SIZE)) {
		discard;
	}

	color = vec4(LINE_DASH_COLOR, 0.5);
}