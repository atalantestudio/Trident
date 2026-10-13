// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	using lyah::vec2;
	using lyah::vec3;
	using lyah::vec4;
	using lyah::mat2x2;
	using lyah::mat3x3;
	using lyah::mat4x3;
	using lyah::mat4x4;
	using lyah::quat;

	using lyah::abs;
	using lyah::ceil;
	using lyah::ceilPowerOf2;
	using lyah::clamp;
	using lyah::cos;
	using lyah::epsilon;
	using lyah::floor;
	using lyah::identity;
	using lyah::inverse;
	using lyah::lerp;
	using lyah::lookAt;
	using lyah::max;
	using lyah::min;
	using lyah::parallelogramArea;
	using lyah::perpendicularLeft;
	using lyah::perpendicularRight;
	using lyah::pi;
	using lyah::pow;
	using lyah::radians;
	using lyah::rotationAngle;
	using lyah::rotationAxisAngle;
	using lyah::round;
	using lyah::scaling;
	using lyah::sin;
	using lyah::sqrt;
	using lyah::tau;
	using lyah::translation;
	using lyah::transpose;
	using lyah::uniformScaling;

	typedef vec2<uint32> Extent2D;
	typedef vec3<uint32> Extent3D;

	struct Plane2D {
		vec2<float32> normal;
		float32 offset;
	};

	// TODO(lyah/vec2/cross-operations): Add to Lyah as `triple`.
	template<typename T>
	inline vec2<T> triple(vec2<T> a, vec2<T> b, vec2<T> c) {
		const vec2<T> b_c = {b.x * c.y, b.y * c.x};

		return {a.y * (b_c.x - b_c.y), a.x * (b_c.y - b_c.x)};
	}

	// TODO(lyah/vec2/cross-operations): Add to Lyah as `crossLength`.
	template<typename T>
	inline vec2<T> crossLength(vec2<T> a, T b) {
		return {a.y * b, a.x * -b};
	}

	template<typename T>
	inline vec2<T> operator*(vec2<T> a, const mat3x3<T>& b) {
		return {
			a.x * b[0].x + a.y * b[1].x + b[2].x,
			a.x * b[0].y + a.y * b[1].y + b[2].y,
		};
	}

	template<typename T>
	inline vec3<T> operator*(vec3<T> a, const mat4x4<T>& b) {
		return {
			a.x * b[0].x + a.y * b[1].x + a.z * b[2].x + b[3].x,
			a.x * b[0].y + a.y * b[1].y + a.z * b[2].y + b[3].y,
			a.x * b[0].z + a.y * b[1].z + a.z * b[2].z + b[3].z,
		};
	}

	/// Calculates the signed distance between a point and an infinite line represented by two points.
	inline float32 signedDistanceLPAB(vec2<float32> a, vec2<float32> b, vec2<float32> p) {
		const vec2<float32> ab = b - a;
		const vec2<float32> ap = p - a;

		return parallelogramArea(ab, ap) / length(ab);
	}

	/// Calculates the signed distance between a point and an infinite plane.
	inline float32 signedDistance(vec2<float32> point, Plane2D plane) {
		return dot(point, plane.normal) - plane.offset;
	}

	/// Calculates the signed distance between a point and an infinite line represented by a normal and a vector offset.
	inline float32 signedDistanceLP(vec2<float32> normal, vec2<float32> offset, vec2<float32> point) {
		return dot(normal, point - offset);
	}

	/// Calculates the signed distance between a point and an infinite plane represented by a normal and an offset.
	inline float32 signedDistanceFP(vec3<float32> normal, float32 offset, vec3<float32> point) {
		return dot(normal, point) - offset;
	}

	inline vec2<float32> edgeNormalClockwise(vec2<float32> a, vec2<float32> b) {
		return perpendicularLeft(normalized(b - a));
	}

	inline bool parallelOrOpposite(vec3<float32> a, vec3<float32> b) {
		constexpr float32 E = 0.0001f;

		if (dot(a, b) / sqrt(lengthSquared(a) * lengthSquared(b)) > 1.0f - E) {
			return true;
		}

		if (dot(a, -b) / sqrt(lengthSquared(a) * lengthSquared(b)) > 1.0f - E) {
			return true;
		}

		return false;
	}

	inline mat3x3<float32> orthographicTopLeftOrigin(vec2<float32> viewport) {
		return {
			 2.0f / viewport[0],  0.0f,                0.0f,
			 0.0f,               -2.0f / viewport[1],  0.0f,
			-1.0f,                1.0f,                1.0f,
		};
	}

	/// Calculates and returns a perspective projection matrix.
	inline mat4x4<float32> perspective(
		/// Vertical field of view in radians.
		float32 fieldOfView,

		float32 aspectRatio,
		float32 nearPlane,
		float32 farPlane,

		/// Coordinate system sign. 1 for left-handed and -1 for right-handed.
		float32 C

		// float32 bias = pi<float32>() * 0.5f
	) {
		DEBUG_ASSERT(nearPlane > 0.0f);
		DEBUG_ASSERT(farPlane >= nearPlane);

		const float32 f = tan(/* bias - */ fieldOfView * 0.5f);

		return {
			 1.0f / (aspectRatio * f),  0.0f,         0.0f,                                               0.0f,
			 0.0f,                      1.0f / f,     0.0f,                                               0.0f,
			 0.0f,                      0.0f,         farPlane / (farPlane - nearPlane),                  C,
			 0.0f,                      0.0f,        -C * nearPlane * farPlane / (farPlane - nearPlane),  0.0f,
		};
	}

	/// Calculates and returns a Reversed-Z perspective projection matrix (flips the depth and defines an infinite far plane).
	/// See https://nlguillemot.wordpress.com/2016/12/07/reversed-z-in-opengl.
	inline mat4x4<float32> reversedZ(
		/// Vertical field of view in radians.
		float32 fieldOfView,

		float32 aspectRatio,
		float32 nearPlane,

		/// Coordinate system sign. 1 for left-handed and -1 for right-handed.
		float32 C
	) {
		DEBUG_ASSERT(nearPlane > 0.0f);

		const float32 f = 1.0f / tan(fieldOfView * 0.5f);

		return {
			f / aspectRatio, 0.0f, 0.0f,      0.0f,
			0.0f,            f,    0.0f,      0.0f,
			0.0f,            0.0f, 0.0f,      C,
			0.0f,            0.0f, nearPlane, 0.0f,
		};
	}

	/// See https://stackoverflow.com/a/17654730/17136841.
	inline quat<float32> lookAt(vec3<float32> previousDirection, vec3<float32> direction, vec3<float32> up) {
		static constexpr float32 e = 1e-5f;

		const float32 angle = dot(previousDirection, direction);

		if (abs(angle + 1.0f) < e) {
			return rotationAxisAngle<quat<float32>>(up, pi<float32>());
		}

		if (abs(angle - 1.0f) < e) {
			return identity<quat<float32>>();
		}

		const float32 rotationAngle = acos(angle);

		vec3<float32> rotationAxis = cross(previousDirection, direction);

		if (lengthSquared(rotationAxis) > e) {
			rotationAxis = normalized(rotationAxis);
		}

		return rotationAxisAngle<quat<float32>>(rotationAxis, rotationAngle);
	}

	inline float32 fract(float32 a) {
		return a - static_cast<sint32>(a);
	}
}