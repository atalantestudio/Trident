// Copyright 2025 Matteo Legagneux.

#version 450

// Spacing between columns and rows, in px.
const vec2 SPACING = vec2(1.0, 1.0);

// Color of the background.
const vec3 BACKGROUND_COLOR = vec3(1.0, 1.0, 1.0);

// Color of the rows and columns.
const vec3 LINE_COLOR = vec3(0.867, 0.867, 0.867);

// Color of the horizontal and vertical axes.
const vec3 AXIS_COLOR = vec3(0.733, 0.733, 0.733);

in VertexOutput {
	// Texture coordinate in the range [-1, 1].
	layout(location = 0) vec2 textureCoordinate;
} vertexOutput;

layout(set = 0, binding = 0) uniform Camera {
	// Spacing multiplier, in px.
	float zoomLevel;
} camera;

layout(set = 0, binding = 1) uniform View {
	// Viewport size, in px.
	vec2 viewport;
} view;

layout(location = 0) out vec4 color;

void main() {
	color.rgb = BACKGROUND_COLOR;
	color.a = 1.0;

	vec2 viewportHalf = view.viewport * 0.5;
	viewportHalf.x = floor(viewportHalf.x);
	viewportHalf.y = ceil(viewportHalf.y);

	vec2 coordinate = vertexOutput.textureCoordinate * viewportHalf;

	ivec2 spacing = ivec2(SPACING * camera.zoomLevel);

	vec2 line = floor(mod(coordinate, spacing));

	if (min(line.x, line.y) == 0.0) {
		color.rgb = LINE_COLOR;

		vec2 axis = abs(floor(coordinate));

		if (min(axis.x, axis.y) == 0.0) {
			color.rgb = AXIS_COLOR;
		}
	}
}