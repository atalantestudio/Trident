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

struct Sphere {
	vec3 position;
	float radius;
};

struct Hit {
	float t0;
	float t1;
};

const float EPSILON = 0.0001;
const float FLT_MAX = 4294967295.0;

const vec3 UP = vec3(0.0, 1.0, 0.0);

const Sphere EARTH = Sphere(vec3(0.0, 0.0, 0.0), 0.4);
const Sphere EARTH_ATMOSPHERE = Sphere(vec3(0.0, 0.0, 0.0), 1.0);
const Sphere SUN = Sphere(vec3(0.0, 1000.0, 0.0), 10.0);
const vec3 WAVE_LENGTHS = vec3(630.0, 532.0, 465.0);
const float SCATTERING_STRENGTH = 12.0;
const vec3 SCATTERING_COEFFICIENTS = vec3(
	pow(400.0 / WAVE_LENGTHS.r, 4.0) * SCATTERING_STRENGTH,
	pow(400.0 / WAVE_LENGTHS.g, 4.0) * SCATTERING_STRENGTH,
	pow(400.0 / WAVE_LENGTHS.b, 4.0) * SCATTERING_STRENGTH
);

layout(set = 0, binding = 0) uniform readonly CameraBuffer {
	Camera camera;
};
layout(set = 0, binding = 1) buffer readonly MaterialCoordinateBuffer {
	uint materialCoordinateBuffer[];
};
layout(set = 0, binding = 2, rgba8) uniform writeonly image2D outputImage;
layout(set = 1, binding = 0) buffer readonly MaterialOffsetBuffer {
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

Hit intersectRaySphere(Ray ray, Sphere sphere) {
	Hit hit;
	hit.t0 = FLT_MAX;
	hit.t1 = 0.0;

	vec3 origin = ray.origin - sphere.position;
	float a = dot(ray.direction, ray.direction);
	float b = dot(origin, ray.direction);
	float c = dot(origin, origin) - sphere.radius * sphere.radius;
	float discriminant = b * b - a * c;

	if (discriminant > 0.0) {
		float d = sqrt(discriminant);
		float divisor = 1.0 / (2.0 * a);
		float far = (-b + d) * divisor;

		if (far > 0.0) {
			float near = max(0.0, (-b - d) * divisor);

			hit.t0 = near;
			hit.t1 = far - near;
		}
	}

	return hit;
}

float calculateDensity(vec3 p) {
	const float DENSITY_FALLOFF = 20.0;

	float heightAboveSurface = length(p - EARTH.position) - EARTH.radius;
	float height = heightAboveSurface / (EARTH_ATMOSPHERE.radius - EARTH.radius);
	float density = exp(-height * DENSITY_FALLOFF);

	return density;
}

float calculateOpticalDepth(vec3 origin, vec3 direction, float rayLength) {
	const uint SAMPLE_COUNT = 10;

	vec3 samplePosition = origin;
	float sampleStride = rayLength / (SAMPLE_COUNT - 1);
	float opticalDepth = 0.0;
	uint sampleIndex = 0;

	while (sampleIndex < SAMPLE_COUNT) {
		opticalDepth += calculateDensity(samplePosition) * sampleStride;
		samplePosition += direction * sampleStride;
		sampleIndex += 1;
	}

	return opticalDepth;
}

vec3 calculateLight(vec3 origin, vec3 direction, float rayLength) {
	const uint SAMPLE_COUNT = 10;

	vec3 dirToSun = normalize(SUN.position - origin);

	Ray sampleRay;
	sampleRay.origin = origin;
	sampleRay.direction = dirToSun;

	float sampleStride = rayLength / (SAMPLE_COUNT - 1);
	vec3 light = vec3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	while (sampleIndex < SAMPLE_COUNT) {
		float density = calculateDensity(sampleRay.origin);
		float sunRayLength = intersectRaySphere(sampleRay, EARTH_ATMOSPHERE).t1;

		// Sample to sun
		float sunRayOpticalDepth = calculateOpticalDepth(sampleRay.origin, dirToSun, sunRayLength);

		// Sample to camera
		float viewRayOpticalDepth = calculateOpticalDepth(sampleRay.origin, -direction, sampleIndex * sampleStride);

		vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * SCATTERING_COEFFICIENTS);

		light += density * transmittance * SCATTERING_COEFFICIENTS * sampleStride;
		sampleRay.origin += direction * sampleStride;

		sampleIndex += 1;
	}

	return light;
}

vec3 Material(uvec2 screenCoordinate) {
	Ray ray;
	ray.origin = EARTH.position + UP * (EARTH.radius + EPSILON);
	ray.direction = calculateRayDirection(screenCoordinate);

	Hit earthHit = intersectRaySphere(ray, EARTH);
	Hit atmosphereHit = intersectRaySphere(ray, EARTH_ATMOSPHERE);

	float dstThroughAtmosphere = min(earthHit.t0 - atmosphereHit.t0, atmosphereHit.t1);

	if (earthHit.t1 <= 0.0) {
		Hit sunHit = intersectRaySphere(ray, SUN);

		if (sunHit.t1 > 0.0) {
			// Return sun color.
			return vec3(1.0);
		}
	}

	if (dstThroughAtmosphere > 0.0) {
		vec3 p = ray.origin + ray.direction * (atmosphereHit.t0 + EPSILON);

		if (earthHit.t0 < FLT_MAX) {
			dstThroughAtmosphere = atmosphereHit.t1;
		}

		return calculateLight(p, ray.direction, dstThroughAtmosphere - EPSILON * 2.0);
	}
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