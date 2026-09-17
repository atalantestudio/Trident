// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace lyah {
	// TODO(lyah/sum): See https://github.com/orgs/atalantestudio/projects/4/views/1?pane=issue&itemId=210334400&issue=atalantestudio%7Clyah%7C27.
	inline std::float_t sum(vec<3, std::float_t> a) {
		return a[0] + a[1] + a[2];
	}

	// vec<2, sint32> polyfill.
	template<>
	struct vec<2, std::int32_t> {
		public:
			inline std::int32_t operator[](std::size_t index) const noexcept {
				DEBUG_ASSERT(index < 2);

				return m[index];
			}

			inline std::int32_t& operator[](std::size_t index) noexcept {
				DEBUG_ASSERT(index < 2);

				return m[index];
			}

		private:
			std::int32_t m[2];
	};

	// vec<3, sint32> polyfill.
	template<>
	struct vec<3, std::int32_t> {
		public:
			inline std::int32_t operator[](std::size_t index) const noexcept {
				DEBUG_ASSERT(index < 3);

				return m[index];
			}

			inline std::int32_t& operator[](std::size_t index) noexcept {
				DEBUG_ASSERT(index < 3);

				return m[index];
			}

		private:
			std::int32_t m[3];
	};

	// vec<2, uint32> polyfill.
	template<>
	struct vec<2, std::uint32_t> {
		public:
			inline std::uint32_t operator[](std::size_t index) const noexcept {
				DEBUG_ASSERT(index < 2);

				return m[index];
			}

			inline std::uint32_t& operator[](std::size_t index) noexcept {
				DEBUG_ASSERT(index < 2);

				return m[index];
			}

		private:
			std::uint32_t m[2];
	};

	// vec<3, uint32> polyfill.
	template<>
	struct vec<3, std::uint32_t> {
		public:
			inline std::uint32_t operator[](std::size_t index) const noexcept {
				DEBUG_ASSERT(index < 3);

				return m[index];
			}

			inline std::uint32_t& operator[](std::size_t index) noexcept {
				DEBUG_ASSERT(index < 3);

				return m[index];
			}

		private:
			std::uint32_t m[3];
	};

	// vec<4, uint32> polyfill.
	template<>
	struct vec<4, std::uint32_t> {
		public:
			inline std::uint32_t operator[](std::size_t index) const noexcept {
				DEBUG_ASSERT(index < 4);

				return m[index];
			}

			inline std::uint32_t& operator[](std::size_t index) noexcept {
				DEBUG_ASSERT(index < 4);

				return m[index];
			}

		private:
			std::uint32_t m[4];
	};
}

namespace Trident {
	template<std::size_t C, typename T>
	using vec = lyah::vec<C, T>;

	template<typename T>
	using vec2 = lyah::vec2<T>;

	template<typename T>
	using vec3 = lyah::vec3<T>;

	template<typename T>
	using vec4 = lyah::vec4<T>;

	template<std::size_t M, std::size_t N, typename T>
	using mat = lyah::mat<M, N, T>;

	template<typename T>
	using mat2x2 = lyah::mat2x2<T>;

	template<typename T>
	using mat3x3 = lyah::mat3x3<T>;

	template<typename T>
	using mat4x4 = lyah::mat4x4<T>;

	template<typename T>
	using quat = lyah::quat<T>;

	template<std::size_t C, typename T>
	struct svec;

	template<typename T>
	struct alignas(2 * sizeof(T)) svec<2, T> {
		T x;
		T y;

		constexpr svec() :
			x(static_cast<T>(0)), y(static_cast<T>(0))
		{}

		constexpr svec(T x, T y) :
			x(x), y(y)
		{}

		// TODO(lyah/scalar): Remove once Lyah supports scalar vector math.
		constexpr explicit svec(vec<2, T> a) :
			x(a[0]),
			y(a[1])
		{}

		inline T operator[](std::size_t index) const {
			DEBUG_ASSERT(index < 2);

			return pointer_cast<const T*>(this)[index];
		}

		inline T& operator[](std::size_t index) {
			DEBUG_ASSERT(index < 2);

			return pointer_cast<T*>(this)[index];
		}

		// TODO(lyah/scalar): Add scalar vectors and replace SIMD vectors by general-purpose registers.
		inline operator vec<2, T>() const {
			return {x, y};
		}
	};

	template<typename T>
	struct svec<3, T> {
		T x;
		T y;
		T z;

		constexpr svec() :
			x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0))
		{}

		constexpr svec(T x, T y, T z) :
			x(x), y(y), z(z)
		{}

		// TODO(lyah/scalar): Remove once Lyah supports scalar vector math.
		constexpr explicit svec(vec<3, T> a) :
			x(a[0]), y(a[1]), z(a[2])
		{}

		inline T operator[](std::size_t index) const {
			DEBUG_ASSERT(index < 3);

			return pointer_cast<const T*>(this)[index];
		}

		inline T& operator[](std::size_t index) {
			DEBUG_ASSERT(index < 3);

			return pointer_cast<T*>(this)[index];
		}

		// TODO(lyah/scalar): Add scalar vectors and replace SIMD vectors by general-purpose registers.
		inline operator vec<3, T>() const {
			return {x, y, z};
		}
	};

	template<typename T>
	constexpr svec<2, T> operator-(svec<2, T> a) {
		return {
			-a.x,
			-a.y,
		};
	}

	template<std::size_t C, typename T>
	constexpr svec<C, T>& operator+=(svec<C, T>& a, const svec<C, T>& b);

	template<typename T>
	constexpr svec<2, T>& operator+=(svec<2, T>& a, const svec<2, T>& b) {
		a.x += b.x;
		a.y += b.y;

		return a;
	}

	template<typename T>
	constexpr bool operator==(svec<2, T> a, svec<2, T> b) {
		return a.x == b.x && a.y == b.y;
	}

	template<typename T>
	constexpr bool operator!=(svec<2, T> a, svec<2, T> b) {
		return a.x != b.x || a.y != b.y;
	}

	template<typename T>
	constexpr svec<2, T>& operator-=(svec<2, T>& a, const svec<2, T>& b) {
		a.x -= b.x;
		a.y -= b.y;

		return a;
	}

	template<typename T>
	constexpr svec<3, T>& operator+=(svec<3, T>& a, const svec<3, T>& b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;

		return a;
	}

	template<typename T>
	constexpr svec<2, T>& operator*=(svec<2, T>& a, T b) {
		a.x *= b;
		a.y *= b;

		return a;
	}

	template<typename T>
	constexpr svec<3, T>& operator*=(svec<3, T>& a, T b) {
		a.x *= b;
		a.y *= b;
		a.z *= b;

		return a;
	}

	template<typename T>
	constexpr svec<3, T> operator*(T b, svec<3, T> a) {
		return a *= b;
	}

	template<std::size_t C, typename T>
	constexpr svec<C, T>& operator*=(svec<C, T>& a, const svec<C, T>& b);

	template<typename T>
	constexpr svec<2, T>& operator*=(svec<2, T>& a, const svec<2, T>& b) {
		a.x *= b.x;
		a.y *= b.y;

		return a;
	}

	template<typename T>
	constexpr svec<3, T>& operator*=(svec<3, T>& a, const svec<3, T>& b) {
		a.x *= b.x;
		a.y *= b.y;
		a.z *= b.z;

		return a;
	}

	template<typename T>
	constexpr svec<2, T>& operator/=(svec<2, T>& a, svec<2, T> b) {
		a.x /= b.x;
		a.y /= b.y;

		return a;
	}

	template<std::size_t C, typename T>
	constexpr svec<C, T>& operator/=(svec<C, T>& a, T b);

	template<typename T>
	constexpr svec<2, T>& operator/=(svec<2, T>& a, T b) {
		a.x /= b;
		a.y /= b;

		return a;
	}

	template<typename T>
	constexpr svec<3, T>& operator/=(svec<3, T>& a, T b) {
		a.x /= b;
		a.y /= b;
		a.z /= b;

		return a;
	}

	template<typename T>
	constexpr svec<2, T> operator/(T b, svec<2, T> a) {
		return svec<2, T>(b, b) / a;
	}

	template<typename T>
	constexpr svec<2, T> operator/(svec<2, T> a, svec<2, T> b) {
		return a /= b;
	}

	template<std::size_t C, typename T>
	constexpr svec<C, T> operator+(svec<C, T> a, svec<C, T> b) {
		return a += b;
	}

	template<std::size_t C, typename T>
	constexpr svec<C, T> operator*(svec<C, T> a, svec<C, T> b) {
		return a *= b;
	}

	template<std::size_t C, typename T>
	constexpr svec<C, T> operator*(svec<C, T> a, T b) {
		return a *= b;
	}

	template<std::size_t C, typename T>
	constexpr svec<C, T> operator*(T b, svec<C, T> a) {
		return a *= b;
	}

	template<std::size_t C, typename T>
	constexpr svec<C, T> operator-(const svec<C, T>& a, const svec<C, T>& b);

	template<typename T>
	constexpr svec<2, T> operator-(const svec<2, T>& a, const svec<2, T>& b) {
		return {a.x - b.x, a.y - b.y};
	}

	template<typename T>
	constexpr svec<3, T> operator-(const svec<3, T>& a, const svec<3, T>& b) {
		return {a.x - b.x, a.y - b.y, a.z - b.z};
	}

	template<typename T>
	using svec2 = svec<2, T>;

	template<typename T>
	using svec3 = svec<3, T>;

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

			sim(const lyah::vec<2, T>& a) :
				m{a},
				count(1)
			{}

			sim(const lyah::vec<2, T>& a, const lyah::vec<2, T>& b) :
				m{a, b},
				count(2)
			{}

			inline const lyah::vec<2, T>& operator[](std::uint8_t index) const {
				DEBUG_ASSERT(index < count);

				return m[index];
			}

			inline lyah::vec<2, T>& operator[](std::uint8_t index) {
				DEBUG_ASSERT(index < 3);

				return m[index];
			}

		public:
			lyah::vec<2, T> m[3];
			std::uint8_t count : 2;
	};

	template<typename T>
	using sim2 = sim<2, T>;

	// Returns the area of the parallelogram formed by `a` and `b`.
	template<typename T>
	inline T parallelogramArea(vec<2, T> a, vec<2, T> b) {
		return a[0] * b[1] - a[1] * b[0];
	}

	template<typename T>
	inline vec<2, T> perpendicularLeft(vec<2, T> a) {
		return {-a[1], a[0]};
	}

	template<typename T>
	inline vec<2, T> perpendicularRight(vec<2, T> a) {
		return {a[1], -a[0]};
	}

	using lyah::abs;
	using lyah::ceil;
	using lyah::clamp;
	using lyah::cos;
	using lyah::epsilon;
	using lyah::infinity;
	using lyah::inverse;
	using lyah::lerp;
	using lyah::max;
	using lyah::min;
	using lyah::pi;
	using lyah::pow;
	using lyah::radians;
	using std::round;
	using lyah::sin;
	using lyah::sqrt;
	using lyah::tau;
	using lyah::transpose;

	template<typename T>
	inline constexpr T pow_constexpr(T base, std::size_t exp) {
		DEBUG_ASSERT(exp < 8);

		if (exp == 0) {
			return static_cast<T>(1);
		}

		return base * pow_constexpr(base, exp - 1);
	}

	template<std::size_t C, typename T>
	inline vec<C, T> round(vec<C, T> a);

	template<typename T>
	inline vec<2, T> round(vec<2, T> a) {
		return {round(a[0]), round(a[1])};
	}

	template<typename T>
	inline vec<3, T> round(vec<3, T> a) {
		return {round(a[0]), round(a[1]), round(a[2])};
	}

	template<std::size_t E, std::size_t C, typename T>
	inline vec<C, T> round(vec<C, T> a) {
		constexpr T p = pow_constexpr(static_cast<T>(10), E);

		return round(a * p) / p;
	}

	// TODO(lyah/scaled-area)
	// Access the vertices using the indices provided by the geometry.
	// Returns the absolute area of a 2D polygon defined by a sequence of vertices.
	// See https://stackoverflow.com/a/717367/17136841.
	template<typename T>
	T scaledArea(view<vec<2, T>> unclosedVertices, vec2<float32> scale) {
		T area = static_cast<T>(0);

		// Close vertex array.
		sequence<vec<2, T>> closedVertices(unclosedVertices.count() + 2);

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
	inline lyah::vec<2, T> cross2d(const lyah::vec<2, T>& a, const lyah::vec<2, T>& b, const lyah::vec<2, T>& c) {
		const lyah::vec<2, T> b_c = {b[0] * c[1], b[1] * c[0]};

		return {
			a[1] * (b_c[0] - b_c[1]),
			a[0] * (b_c[1] - b_c[0]),
		};
	}

	template<typename T>
	inline lyah::vec<2, T> multiplyTransform(const lyah::vec<2, T>& a, const lyah::mat<3, 3, T>& t) {
		return {
			t[0][0] * a[0] + t[1][0] * a[1] + t[2][0],
			t[0][1] * a[0] + t[1][1] * a[1] + t[2][1],
		};
	}

	template<typename T>
	inline vec<3, T> multiplyTransform(vec<3, T> a, const mat<4, 4, T>& M) {
		const vec<4, T> r = vec<4, T>(a[0], a[1], a[2], 1.0f) * M;

		return {r[0], r[1], r[2]};
	}

	template<typename T>
	inline svec2<T> abs(svec2<T> a) {
		return {
			abs(a.x),
			abs(a.y),
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

	inline vec2<float32> edgeNormalClockwise(const vec2<float32>& a, const vec2<float32>& b) {
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

	inline float32 avg(float32 a, float32 b) {
		return (a + b) * 0.5f;
	}

	template<typename T>
	inline T ceil(T value, T multiplier) {
		return (value + multiplier - 1) & ~(multiplier - 1);
	}

	inline float32 fract(float32 a) {
		return a - static_cast<sint32>(a);
	}

	// TODO(lyah): Fix crash when inverting the matrix below.
	// {
	//    -1.19209290e-07f,  0.197657436f,     0.980271161f,     0.00000000f,
	//     7.45058060e-09f,  0.980271161f,    -0.197657421f,     0.00000000f,
	//    -1.00000000f,     -2.98023224e-08f, -1.19209290e-07f,  0.00000000f,
	//    -4.00000000f,     -0.882244110f,     0.177891061f,     1.00000000f,
	// }
	inline mat4x4<float32> inverseFix(mat4x4<float32> m) {
		float32 Coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
		float32 Coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
		float32 Coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

		float32 Coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
		float32 Coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
		float32 Coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

		float32 Coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
		float32 Coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
		float32 Coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

		float32 Coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
		float32 Coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
		float32 Coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

		float32 Coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
		float32 Coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
		float32 Coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

		float32 Coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
		float32 Coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
		float32 Coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

		vec<4, float32> Fac0(Coef00, Coef00, Coef02, Coef03);
		vec<4, float32> Fac1(Coef04, Coef04, Coef06, Coef07);
		vec<4, float32> Fac2(Coef08, Coef08, Coef10, Coef11);
		vec<4, float32> Fac3(Coef12, Coef12, Coef14, Coef15);
		vec<4, float32> Fac4(Coef16, Coef16, Coef18, Coef19);
		vec<4, float32> Fac5(Coef20, Coef20, Coef22, Coef23);

		vec<4, float32> Vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
		vec<4, float32> Vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
		vec<4, float32> Vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
		vec<4, float32> Vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

		vec<4, float32> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
		vec<4, float32> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
		vec<4, float32> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
		vec<4, float32> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

		vec<4, float32> SignA(+1, -1, +1, -1);
		vec<4, float32> SignB(-1, +1, -1, +1);
		mat<4, 4, float32> Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

		vec<4, float32> Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

		vec<4, float32> Dot0(m[0] * Row0);
		float32 Dot1 = (Dot0[0] + Dot0[1]) + (Dot0[2] + Dot0[3]);
		float32 OneOverDeterminant = 1.0f / Dot1;

		return Inverse * OneOverDeterminant;
	}
}