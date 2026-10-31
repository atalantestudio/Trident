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
	using lyah::orthographic;
	using lyah::orthographicScreen;
	using lyah::parallelogramArea;
	using lyah::perpendicularLeft;
	using lyah::perpendicularRight;
	using lyah::perspective;
	using lyah::perspectiveReversedZ;
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

	// TODO(lyah/signed-distance): Add to Lyah.
	/// Calculates the signed distance between a point `p` and an infinite line represented by `a` and `b`.
	inline float32 signedDistanceLPAB(vec2<float32> a, vec2<float32> b, vec2<float32> p) {
		const vec2<float32> ab = b - a;
		const vec2<float32> ap = p - a;

		return parallelogramArea(ab, ap) / length(ab);
	}

	// See https://stackoverflow.com/a/36425155/17136841.
	inline float32 signedDistanceLPAB(vec3<float32> a, vec3<float32> b, vec3<float32> p) {
		const vec3<float32> ab = b - a;
		const vec3<float32> ap = p - a;

		if (dot(ab, ap) < 0.0f) {
			return length(ap);
		}

		const vec3<float32> bp = p - b;

		if (dot(bp, ab) > 0.0f) {
			return length(bp);
		}

		return length(cross(ab, ap)) / length(ab);
	}

	// TODO(lyah/signed-distance): Add to Lyah.
	/// Calculates the signed distance between a point and an infinite plane.
	inline float32 signedDistance(vec2<float32> point, Plane2D plane) {
		return dot(point, plane.normal) - plane.offset;
	}

	// TODO(lyah/signed-distance): Add to Lyah.
	/// Calculates the signed distance between a point and an infinite line represented by a normal and a vector offset.
	inline float32 signedDistanceLP(vec2<float32> normal, vec2<float32> offset, vec2<float32> point) {
		return dot(normal, point - offset);
	}

	// TODO(lyah/signed-distance): Add to Lyah.
	/// Calculates the signed distance between a point and an infinite plane represented by a normal and an offset.
	inline float32 signedDistanceFP(vec3<float32> normal, float32 offset, vec3<float32> point) {
		return dot(normal, point) - offset;
	}

	inline vec2<float32> edgeNormalClockwise(vec2<float32> a, vec2<float32> b) {
		return perpendicularLeft(normalized(b - a));
	}

	inline vec3<float32> edgeNormalClockwise(vec3<float32> a, vec3<float32> b, vec3<float32> up) {
		return normalized(cross(b - a, up));
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

	// TODO(lyah/quaternion): Add to Lyah as `rotation` or `rotationFromTo`.
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
}