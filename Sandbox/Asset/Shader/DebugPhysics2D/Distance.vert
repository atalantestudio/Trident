#version 450

layout(location = 0) in vec2 vertex;

layout(set = 0, binding = 0) uniform Camera {
	mat3x3 projection;
	float zoomLevel;
} camera;

out VertexOutput {
	layout(location = 0) flat vec2 startPosition;
	layout(location = 1) vec2 currentPosition;
} vertexOutput;

void main() {
	vec3 position = camera.projection * vec3(vertex, 1.0) * camera.zoomLevel;

	gl_Position = vec4(position.xy, 0.0, 1.0);

	vertexOutput.startPosition = gl_Position.xy / gl_Position.w;
	vertexOutput.currentPosition = vertexOutput.startPosition;
}