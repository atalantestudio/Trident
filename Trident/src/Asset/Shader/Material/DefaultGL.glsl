#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct Camera {
	mat4x4 projectionView;
	float nearPlane;
	float farPlane;
	uvec2 viewport;
	vec3 position;
};

struct Range {
	uint start;
	uint end;
};

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Hit {
	float t0;
	float t1;
};

const float PI = 3.141592;

layout(binding = 0) uniform readonly CameraBuffer {
	Camera camera;
};
layout(binding = 1) buffer readonly MaterialCoordinateBuffer {
	uint materialCoordinateBuffer[];
};
layout(binding = 12, rgba8) uniform writeonly image2D outputImage;
layout(binding = 13) buffer readonly MaterialOffsetBuffer {
	Range materialShaderCoordinateIndexRange;
};

vec3 calculateRayDirection(uvec2 screenCoordinate) {
	vec2 normalizedDeviceCoordinate = vec2(screenCoordinate) / camera.viewport * 2.0 - 1.0;
	normalizedDeviceCoordinate.y *= -1.0;

	mat4x4 projectionView = camera.projectionView;
	projectionView[3] = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 far = inverse(projectionView) * vec4(normalizedDeviceCoordinate, 1.0, 1.0);

	return normalize(far.xyz / far.w);
}

Hit intersectRaySphere(Ray ray, float sphereRadius) {
	Hit hit;
	hit.t0 = 1e5;
	hit.t1 = -1e5;

	float a = dot(ray.direction, ray.direction);
	float b = dot(ray.origin, ray.direction) * 2.0;
	float c = dot(ray.origin, ray.origin) - sphereRadius * sphereRadius;
	float discriminant = b * b - 4.0 * a * c;

	if (discriminant >= 0.0) {
		float d = sqrt(discriminant);
		float divisor = 1.0 / (2.0 * a);

		hit.t0 = (-b - d) * divisor;
		hit.t1 = (-b + d) * divisor;
	}

	return hit;
}

// TODO: Replicate on the other default material shaders.
vec3 sampleAtmosphere(
	Ray ray,
	vec3 sunPosition,
	float sunIntensity,
	float planetRadius,
	float atmosphereRadius,
	vec3 rayleighScatteringCoefficient,
	float rayleighScaleHeight,
	float mieScatteringCoefficient,
	float mieScaleHeight,
	float miePreferredScatteringDirection
) {
	const uint PRIMARY_RAY_STEP_COUNT = 16;
	const uint SECONDARY_RAY_STEP_COUNT = 8;

	Hit atmosphereHit = intersectRaySphere(ray, atmosphereRadius);

	if (atmosphereHit.t0 > atmosphereHit.t1) {
		return vec3(0.0, 0.0, 0.0);
	}

	atmosphereHit.t1 = min(atmosphereHit.t1, intersectRaySphere(ray, planetRadius).t0);

	Ray primaryRay;
	primaryRay.direction = normalize(sunPosition);

	// Total Rayleigh scattering.
	vec3 R = vec3(0.0, 0.0, 0.0);

	// Total Mie scattering.
	vec3 M = vec3(0.0, 0.0, 0.0);

	float primaryRayStepSize = (atmosphereHit.t1 - atmosphereHit.t0) / PRIMARY_RAY_STEP_COUNT;
	float primaryRayRayleighOpticalDepth = 0.0;
	float primaryRayMieOpticalDepth = 0.0;
	uint primaryRayStepIndex = 0;

	while (primaryRayStepIndex < PRIMARY_RAY_STEP_COUNT) {
		primaryRay.origin = ray.origin + ray.direction * (primaryRayStepSize * (primaryRayStepIndex + 0.5));

		float secondaryRayStepSize = intersectRaySphere(primaryRay, atmosphereRadius).t1 / SECONDARY_RAY_STEP_COUNT;
		float secondaryRayRayleighOpticalDepth = 0.0;
		float secondaryRayMieOpticalDepth = 0.0;
		uint secondaryRayStepIndex = 0;

		while (secondaryRayStepIndex < SECONDARY_RAY_STEP_COUNT) {
			vec3 secondaryRayOrigin = primaryRay.origin + primaryRay.direction * (secondaryRayStepSize * (secondaryRayStepIndex + 0.5));
			float secondaryRayHeight = length(secondaryRayOrigin) - planetRadius;

			secondaryRayRayleighOpticalDepth += exp(-secondaryRayHeight / rayleighScaleHeight) * secondaryRayStepSize;
			secondaryRayMieOpticalDepth += exp(-secondaryRayHeight / mieScaleHeight) * secondaryRayStepSize;

			secondaryRayStepIndex += 1;
		}

		float primaryRayHeight = length(primaryRay.origin) - planetRadius;
		float rayleighOpticalDepthStep = exp(-primaryRayHeight / rayleighScaleHeight) * primaryRayStepSize;
		float mieOpticalDepthStep = exp(-primaryRayHeight / mieScaleHeight) * primaryRayStepSize;

		primaryRayRayleighOpticalDepth += rayleighOpticalDepthStep;
		primaryRayMieOpticalDepth += mieOpticalDepthStep;

		vec3 attenuation = exp(-(
			rayleighScatteringCoefficient * (primaryRayRayleighOpticalDepth + secondaryRayRayleighOpticalDepth) +
			mieScatteringCoefficient * (primaryRayMieOpticalDepth + secondaryRayMieOpticalDepth)
		));

		R += rayleighOpticalDepthStep * attenuation;
		M += mieOpticalDepthStep * attenuation;

		primaryRayStepIndex += 1;
	}

	// TODO
	float mu = dot(ray.direction, primaryRay.direction);
	float mumu = mu * mu;
	float gg = miePreferredScatteringDirection * miePreferredScatteringDirection;

	float rayleigh = 3.0 / (PI * 16.0) * (1.0 + mumu);
	float mie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * miePreferredScatteringDirection, 1.5) * (2.0 + gg));

	return (rayleighScatteringCoefficient * rayleigh * R + mieScatteringCoefficient * mie * M) * sunIntensity;
}

vec3 Material(uvec2 screenCoordinate) {
	Ray ray;
	ray.origin = vec3(0.0, 6372e3, 0.0);
	ray.direction = calculateRayDirection(screenCoordinate);

	vec3 color = sampleAtmosphere(
		ray, // Ray
		vec3(0.0, 0.1, 1.0), // Sun position
		22.0, // Sun intensity
		6371e3, // Planet radius
		6471e3, // Atmosphere radius
		vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
		8e3, // Rayleigh scale height
		21e-6, // Mie scattering coefficient,
		1.2e3, // Mie scale height,
		0.758 // Mie preferred scattering direction
	);

	return 1.0 - exp(-1.0 * color);
}

void main() {
	if (gl_GlobalInvocationID.x >= materialShaderCoordinateIndexRange.end) {
		return;
	}

	uint pixelIndex = materialShaderCoordinateIndexRange.start + gl_GlobalInvocationID.x;
	uint packedCoordinate = materialCoordinateBuffer[pixelIndex];
	uvec2 screenCoordinate = uvec2(packedCoordinate >> 16, packedCoordinate & 0xffffu);

	vec3 color = Material(screenCoordinate);

	imageStore(outputImage, ivec2(screenCoordinate), vec4(color, 1.0));
}