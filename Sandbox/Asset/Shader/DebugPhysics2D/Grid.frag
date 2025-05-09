#version 450

const float SPACING = 1.0; // In pixels.

const vec3 BACKGROUND_COLOR = vec3(1.0, 1.0, 1.0);
const vec3 LINE_COLOR = vec3(0.867, 0.867, 0.867);
const vec3 AXIS_COLOR = vec3(0.733, 0.733, 0.733);

in VertexOutput {
	layout(location = 0) vec2 textureCoordinate; // In the range [-0.5, 0.5].
} vertexOutput;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
	float zoomLevel;
} camera;

layout(set = 0, binding = 1) uniform View {
	vec2 viewport;
} view;

layout(location = 0) out vec4 color;

void main() {
	// Get the current fragment coordinate in the range [-viewport / 2, viewport].
	ivec2 coordinate = ivec2(floor(vertexOutput.textureCoordinate * view.viewport + 0.5));

	int spacing = int(floor(SPACING * camera.zoomLevel));

	vec2 line = mod(coordinate, spacing);
	bool isLine = min(line.x, line.y) == 0.0;

	ivec2 axis = abs(coordinate);
	bool isAxis = min(axis.x, axis.y) == 0;

	color.rgb = float(isLine && !isAxis) * LINE_COLOR;
	color.rgb += float(isAxis) * AXIS_COLOR;
	color.rgb += (1.0 - float(isLine)) * BACKGROUND_COLOR;
}