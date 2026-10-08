// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	using lyah::vec2;
	using lyah::vec3;
	using lyah::vec4;
	using lyah::mat2x2;
	using lyah::mat3x3;
	using lyah::mat4x4;
	using lyah::quat;

	// TODO(lyah): Remove.
	template<typename T>
	using svec2 = lyah::vec<2, T>;

	// TODO(lyah): Remove.
	template<typename T>
	using svec3 = lyah::vec<3, T>;

	using lyah::abs;
	using lyah::axisAngle;
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

	struct Plane2D {
		vec2<float32> normal;
		float32 offset;
	};

	// K-th dimensional simplex.
	template<std::size_t K, typename T>
	struct sim;

	template<typename T>
	struct sim<2, T> {
		public:
			sim() :
				count(0)
			{}

			sim(vec2<T> a) :
				m{a},
				count(1)
			{}

			sim(vec2<T> a, vec2<T> b) :
				m{a, b},
				count(2)
			{}

			inline vec2<T> operator[](std::uint8_t index) const {
				DEBUG_ASSERT(index < count);

				return m[index];
			}

			inline vec2<T>& operator[](std::uint8_t index) {
				DEBUG_ASSERT(index < 3);

				return m[index];
			}

		public:
			vec2<T> m[3];
			std::uint8_t count : 2;
	};

	template<typename T>
	using sim2 = sim<2, T>;

	// TODO(lyah/scaled-area)
	// Access the vertices using the indices provided by the geometry.
	// Returns the absolute area of a 2D polygon defined by a sequence of vertices.
	// See https://stackoverflow.com/a/717367/17136841.
	template<typename T>
	T scaledArea(view<vec2<T>> unclosedVertices, vec2<float32> scale) {
		sequence<vec2<T>> closedVertices(unclosedVertices.count() + 2);
		T area = static_cast<T>(0);

		std::copy(unclosedVertices.begin(), unclosedVertices.end(), &closedVertices[0]);

		closedVertices[unclosedVertices.count() + 0] = unclosedVertices[0];
		closedVertices[unclosedVertices.count() + 1] = unclosedVertices[1];

		uint32 vertexIndex = 1;

		while (vertexIndex <= unclosedVertices.count()) {
			area += closedVertices[vertexIndex][0] * scale[0] * (closedVertices[vertexIndex + 1][1] * scale[1] - closedVertices[vertexIndex - 1][1] * scale[1]);

			vertexIndex += 1;
		}

		return abs(area) * static_cast<T>(0.5);
	}

	template<typename T>
	inline vec2<T> cross2d(vec2<T> a, vec2<T> b, vec2<T> c) {
		const vec2<T> b_c = {b.x * c.y, b.y * c.x};

		return {a.y * (b_c.x - b_c.y), a.x * (b_c.y - b_c.x)};
	}

	// TODO(lyah): Add to Lyah as `cross` or `determinant`.
	template<typename T>
	inline vec2<T> crossVec2Scalar(vec2<T> a, T b) {
		return {a.y * b, a.x * -b};
	}

	template<typename T>
	inline vec2<T> multiplyTransform(vec2<T> a, const mat3x3<T>& t) {
		return {
			t[0][0] * a[0] + t[1][0] * a[1] + t[2][0],
			t[0][1] * a[0] + t[1][1] * a[1] + t[2][1],
		};
	}

	template<typename T>
	inline vec3<T> multiplyTransform(vec3<T> a, const mat4x4<T>& M) {
		const vec4<T> r = vec4<T>(a.x, a.y, a.z, 1.0f) * M;

		return {r.x, r.y, r.z};
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
			return axisAngle<float32>(up, pi<float32>());
		}

		if (abs(angle - 1.0f) < e) {
			return identity<quat<float32>>();
		}

		const float32 rotationAngle = acos(angle);

		vec3<float32> rotationAxis = cross(previousDirection, direction);

		if (lengthSquared(rotationAxis) > e) {
			rotationAxis = normalized(rotationAxis);
		}

		return axisAngle<float32>(rotationAxis, rotationAngle);
	}

	inline float32 fract(float32 a) {
		return a - static_cast<sint32>(a);
	}
}