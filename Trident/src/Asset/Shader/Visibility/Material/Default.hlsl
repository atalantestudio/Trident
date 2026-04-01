// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Camera {
	float4x4 projectionView;
	float nearPlane;
	float farPlane;
	uint2 viewport;
	float3 position;
};

struct Ray {
	float3 origin;
	float3 direction;
};

struct Sphere {
	float3 position;
	float radius;
};

struct Hit {
	float t0;
	float t1;
};

// See https://gist.github.com/mattatz/86fff4b32d198d0928d0fa4ff32cf6fa.
float4x4 inverse(float4x4 m) {
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    float4x4 ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}

Sphere createSphere(float3 position, float radius) {
	Sphere sphere;
	sphere.position = position;
	sphere.radius = radius;

	return sphere;
}

static const uint MATERIAL_INDEX = 0;
static const float FLT_MAX = 4294967295.0;

static const float3 UP = float3(0.0, 1.0, 0.0);
static const Sphere EARTH = createSphere(float3(0.0, 0.0, 0.0), 0.4);
static const Sphere EARTH_ATMOSPHERE = createSphere(float3(0.0, 0.0, 0.0), 1.0);
static const Sphere SUN = createSphere(float3(0.0, 1000.0, 0.0), 10.0);
static const float3 SUN_COLOR = float3(1.0, 0.95, 0.87);
static const float EPSILON = 0.0001;
static const float3 WAVE_LENGTHS = float3(630.0, 532.0, 465.0);
static const float SCATTERING_STRENGTH = 12.0;
static const float3 SCATTERING_COEFFICIENTS = float3(
	pow(400.0 / WAVE_LENGTHS.r, 4.0) * SCATTERING_STRENGTH,
	pow(400.0 / WAVE_LENGTHS.g, 4.0) * SCATTERING_STRENGTH,
	pow(400.0 / WAVE_LENGTHS.b, 4.0) * SCATTERING_STRENGTH
);

cbuffer Camera : register(b0) {
	Camera camera;
}

ByteAddressBuffer materialOffsetBuffer : register(t0);
ByteAddressBuffer materialCoordinateBuffer : register(t1);
RWTexture2D<float4> outputImage : register(u0);

float3 calculateRayDirection(uint2 screenCoordinate) {
	float2 normalizedDeviceCoordinate = float2(screenCoordinate) / camera.viewport * 2.0 - 1.0;
	normalizedDeviceCoordinate.y *= -1.0;

	float4x4 projectionView = transpose(camera.projectionView);
	projectionView[3] = float4(0.0, 0.0, 0.0, 1.0);

	float4 far = mul(inverse(projectionView), float4(normalizedDeviceCoordinate, 1.0, 1.0));

	return normalize(far.xyz / far.w);
}

Hit intersectRaySphere(Ray ray, Sphere sphere) {
	Hit hit;
	hit.t0 = FLT_MAX;
	hit.t1 = 0.0;

	float3 origin = ray.origin - sphere.position;
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

float calculateDensity(float3 p) {
	const float DENSITY_FALLOFF = 20.0;

	float heightAboveSurface = length(p - EARTH.position) - EARTH.radius;
	float height = heightAboveSurface / (EARTH_ATMOSPHERE.radius - EARTH.radius);
	float density = exp(-height * DENSITY_FALLOFF);

	return density;
}

float calculateOpticalDepth(float3 origin, float3 direction, float rayLength) {
	const uint SAMPLE_COUNT = 10;

	float3 samplePosition = origin;
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

float3 calculateLight(float3 origin, float3 direction, float rayLength) {
	const uint SAMPLE_COUNT = 10;

	float3 dirToSun = normalize(SUN.position - origin);

	Ray sampleRay;
	sampleRay.origin = origin;
	sampleRay.direction = dirToSun;

	float sampleStride = rayLength / (SAMPLE_COUNT - 1);
	float3 light = float3(0.0, 0.0, 0.0);
	uint sampleIndex = 0;

	while (sampleIndex < SAMPLE_COUNT) {
		float density = calculateDensity(sampleRay.origin);
		float sunRayLength = intersectRaySphere(sampleRay, EARTH_ATMOSPHERE).t1;

		// Sample to sun
		float sunRayOpticalDepth = calculateOpticalDepth(sampleRay.origin, dirToSun, sunRayLength);

		// Sample to camera
		float viewRayOpticalDepth = calculateOpticalDepth(sampleRay.origin, -direction, sampleIndex * sampleStride);

		float3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * SCATTERING_COEFFICIENTS);

		light += density * transmittance * SCATTERING_COEFFICIENTS * sampleStride;
		sampleRay.origin += direction * sampleStride;

		sampleIndex += 1;
	}

	return light;
}

float3 Material(uint2 screenCoordinate) {
	Ray ray;
	ray.origin = EARTH.position + UP * (EARTH.radius + EPSILON);
	ray.direction = calculateRayDirection(screenCoordinate);

	Hit earthHit = intersectRaySphere(ray, EARTH);
	Hit atmosphereHit = intersectRaySphere(ray, EARTH_ATMOSPHERE);

	float dstThroughAtmosphere = min(earthHit.t0 - atmosphereHit.t0, atmosphereHit.t1);

	if (earthHit.t1 <= 0.0) {
		Hit sunHit = intersectRaySphere(ray, SUN);

		if (sunHit.t1 > 0.0) {
			return SUN_COLOR;
		}
		else {
			return float3(1.0, 1.0, 1.0);
		}
	}
	else if (dstThroughAtmosphere > 0.0) {
		float3 p = ray.origin + ray.direction * (atmosphereHit.t0 + EPSILON);

		if (earthHit.t0 < FLT_MAX) {
			dstThroughAtmosphere = atmosphereHit.t1;
		}

		return calculateLight(p, ray.direction, dstThroughAtmosphere - EPSILON * 2.0);
	}
	else {
		return float3(1.0, 1.0, 1.0);
	}

	return float3(1.0, 1.0, 1.0);
}

[numthreads(256, 1, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	if (globalInvocationIndex.x >= materialOffsetBuffer.Load(MATERIAL_INDEX * 4)) {
		return;
	}

	uint pixelIndex = globalInvocationIndex.x;
	uint packedCoordinate = materialCoordinateBuffer.Load(pixelIndex * 4);
	uint2 screenCoordinate = uint2(packedCoordinate >> 16, packedCoordinate & 0xffff);

	float3 color = Material(screenCoordinate);

	outputImage[screenCoordinate] = float4(color, 1.0);
}