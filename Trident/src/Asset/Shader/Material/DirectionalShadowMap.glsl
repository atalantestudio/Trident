// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

const uint CASCADE_COUNT = 3u;
const float CASCADE_FAR_PLANES[CASCADE_COUNT] = float[](0.2, 0.5, 1.0);

layout(set = 0, binding = 1) uniform readonly DirectionalLight {
	mat4x4 projectionViews[CASCADE_COUNT];
} directionalLight;

layout(set = 0, binding = 3) uniform sampler2DArray directionalShadowMap;

uint findCascadeIndex(vec3 surface) {
	vec4 viewSpaceSurface = camera.view * vec4(surface, 1.0);
	float surfaceViewDepth = abs(viewSpaceSurface.z);
	uint cascadeIndex = 0u;

	while (cascadeIndex < CASCADE_COUNT) {
		if (surfaceViewDepth < camera.farPlane * CASCADE_FAR_PLANES[cascadeIndex]) {
			break;
		}

		cascadeIndex += 1u;
	}

	return cascadeIndex;
}

float calculateShadow(vec3 surface, vec3 normal, uint cascadeIndex) {
	if (cascadeIndex >= CASCADE_COUNT) {
		return 0.0;
	}

	vec4 lightSpaceSurface = directionalLight.projectionViews[cascadeIndex] * vec4(surface, 1.0);
	vec3 projectedPosition = lightSpaceSurface.xyz / lightSpaceSurface.w;

	if (projectedPosition.z > 1.0) {
		return 0.0;
	}

	float depth = projectedPosition.z;

	vec2 coordinate = projectedPosition.xy * 0.5 + 0.5;
	coordinate.y = 1.0 - coordinate.y;

	float depthBias = max(0.05 * (1.0 - dot(normal, SUN_DIRECTION)), 0.005);
	depthBias *= 1.0 / (camera.farPlane * CASCADE_FAR_PLANES[cascadeIndex] * 0.5);

	float closestDepth = texture(directionalShadowMap, vec3(coordinate, cascadeIndex)).r;

	return float(depth - depthBias > closestDepth);
}