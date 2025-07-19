#pragma once

namespace lyah {
	// TODO: Use static arrays instead of class as polyfills.

	// vec<2, sint32> polyfill.
	template<>
	struct lyah::vec<2, std::int32_t> {
		public:
			std::int32_t operator [](std::size_t index) const noexcept {
				DEBUG_ASSERT(index < 2);

				return m[index];
			}

			std::int32_t& operator [](std::size_t index) noexcept {
				DEBUG_ASSERT(index < 2);

				return m[index];
			}

		private:
			std::int32_t m[2];
	};

	// vec<3, sint32> polyfill.
	template<>
	struct lyah::vec<3, std::int32_t> {
		public:
			std::int32_t operator [](std::size_t index) const noexcept {
				DEBUG_ASSERT(index < 3);

				return m[index];
			}

			std::int32_t& operator [](std::size_t index) noexcept {
				DEBUG_ASSERT(index < 3);

				return m[index];
			}

		private:
			std::int32_t m[3];
	};

	// vec<2, uint32> polyfill.
	template<>
	struct lyah::vec<2, std::uint32_t> {
		public:
			std::uint32_t operator [](std::size_t index) const noexcept {
				DEBUG_ASSERT(index < 2);

				return m[index];
			}

			std::uint32_t& operator [](std::size_t index) noexcept {
				DEBUG_ASSERT(index < 2);

				return m[index];
			}

		private:
			std::uint32_t m[2];
	};

	// vec<3, uint32> polyfill.
	template<>
	struct lyah::vec<3, std::uint32_t> {
		public:
			std::uint32_t operator [](std::size_t index) const noexcept {
				DEBUG_ASSERT(index < 3);

				return m[index];
			}

			std::uint32_t& operator [](std::size_t index) noexcept {
				DEBUG_ASSERT(index < 3);

				return m[index];
			}

		private:
			std::uint32_t m[3];
	};

	// vec<4, uint32> polyfill.
	template<>
	struct lyah::vec<4, std::uint32_t> {
		public:
			std::uint32_t operator [](std::size_t index) const noexcept {
				DEBUG_ASSERT(index < 4);

				return m[index];
			}

			std::uint32_t& operator [](std::size_t index) noexcept {
				DEBUG_ASSERT(index < 4);

				return m[index];
			}

		private:
			std::uint32_t m[4];
	};
}

namespace ProjectA {
	template<std::size_t C, typename T>
	using vec = lyah::vec<C, T>;

	template<typename T>
	using vec2 alignas(2 * sizeof(T)) = lyah::vec2<T>;

	template<typename T>
	using vec3 alignas(4 * sizeof(T)) = lyah::vec3<T>;

	template<typename T>
	using vec4 alignas(4 * sizeof(T)) = lyah::vec4<T>;

	template<std::size_t M, std::size_t N, typename T>
	using mat = lyah::mat<M, N, T>;

	template<typename T>
	using mat2x2 = lyah::mat2x2<T>;

	template<typename T>
	using mat3x3 = lyah::mat3x3<T>;

	template<typename T>
	using mat4x4 = lyah::mat4x4<T>;

	template<typename T>
	using quat alignas(4 * sizeof(T)) = lyah::quat<T>;

	// TODO: Expose Lyah methods.

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

		T operator [](std::size_t index) const {
			DEBUG_ASSERT(index < 2);

			return pointer_const_cast<const T*>(this)[index];
		}

		T& operator [](std::size_t index) {
			DEBUG_ASSERT(index < 2);

			return pointer_cast<T*>(this)[index];
		}
	};

	template<typename T>
	struct alignas(4 * sizeof(T)) svec<3, T> {
		T x;
		T y;
		T z;

		constexpr svec() :
			x(static_cast<T>(0)), y(static_cast<T>(0)), y(static_cast<T>(0))
		{}

		constexpr svec(T x, T y, T z) :
			x(x), y(y), z(z)
		{}

		T operator [](std::size_t index) const {
			DEBUG_ASSERT(index < 3);

			return pointer_const_cast<const T*>(this)[index];
		}

		T& operator [](std::size_t index) {
			DEBUG_ASSERT(index < 3);

			return pointer_const<T*>(this)[index];
		}
	};

	template<std::size_t C, typename T>
	constexpr svec<C, T>& operator +=(svec<C, T>& a, const svec<C, T>& b);

	template<typename T>
	constexpr svec<2, T>& operator +=(svec<2, T>& a, const svec<2, T>& b) {
		a.x += b.x;
		a.y += b.y;

		return a;
	}

	template<typename T>
	constexpr svec<3, T>& operator +=(svec<3, T>& a, const svec<3, T>& b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;

		return a;
	}

	template<std::size_t C, typename T>
	constexpr svec<C, T>& operator /=(svec<C, T>& a, T b);

	template<typename T>
	constexpr svec<2, T>& operator /=(svec<2, T>& a, T b) {
		a.x /= b;
		a.y /= b;

		return a;
	}

	template<typename T>
	constexpr svec<3, T>& operator /=(svec<3, T>& a, T b) {
		a.x /= b;
		a.y /= b;
		a.z /= b;

		return a;
	}

	template<std::size_t C, typename T>
	constexpr svec<C, T> operator -(const svec<C, T>& a, const svec<C, T>& b);

	template<typename T>
	constexpr svec<2, T> operator -(const svec<2, T>& a, const svec<2, T>& b) {
		return {b.x - a.x, b.y - a.y};
	}

	template<typename T>
	constexpr svec<3, T> operator -(const svec<3, T>& a, const svec<3, T>& b) {
		return {b.x - a.x, b.y - a.y, b.z - a.z};
	}

	template<std::size_t C, typename T>
	constexpr T lengthSq(const svec<C, T>& a);

	template<typename T>
	constexpr T lengthSq(const svec<2, T>& a) {
		return a.x * a.x + a.y * a.y;
	}

	template<typename T>
	constexpr T lengthSq(const svec<3, T>& a) {
		return a.x * a.x + a.y * a.y + a.z * a.z;
	}

	template<std::size_t C, typename T>
	constexpr T distanceSq(const svec<C, T>& a, const svec<C, T>& b) {
		return lengthSq(b - a);
	}

	template<typename T>
	using svec2 = svec<2, T>;

	// TODO: lyah::lengthSquared
	template<std::size_t C, typename T>
	T lengthSq(const lyah::vec<C, T>& a) {
		return lyah::dot(a, a);
	}

	// TODO: lyah::distanceSquared
	template<std::size_t C, typename T>
	T distanceSq(const lyah::vec<C, T>& a, const lyah::vec<C, T>& b) {
		return lengthSq(b - a);
	}

	// K-th dimensional simplex.
	template<std::size_t K, typename T>
	struct sim;

	template<typename T>
	struct sim<2, T> {
		lyah::vec<2, T> m[3];
		std::uint8_t count : 2;

		sim() : count(0) {}

		sim(const lyah::vec<2, T>& a) : m{a}, count(1) {}

		sim(const lyah::vec<2, T>& a, const lyah::vec<2, T>& b) : m{a, b}, count(2) {}

		const lyah::vec<2, T>& operator [](std::uint8_t index) const {
			DEBUG_ASSERT(index < count);

			return m[index];
		}

		lyah::vec<2, T>& operator [](std::uint8_t index) {
			DEBUG_ASSERT(index < 3);

			return m[index];
		}
	};

	template<typename T>
	using sim2 = sim<2, T>;

	// Returns the area of the parallelogram formed by `a` and `b`.
	template<std::size_t C, typename T>
	T parallelogramArea(const lyah::vec<C, T>& a, const lyah::vec<C, T>& b) {
		return a[0] * b[1] - a[1] * b[0];
	}

	template<typename T>
	lyah::vec<2, T> cross(const lyah::vec<2, T>& a, float32 b) {
		return {b * a[1], -b * a[0]};
	}

	template<typename T>
	lyah::vec<2, T> cross(float32 a, const lyah::vec<2, T>& b) {
		return {-a * b[1], a * b[0]};
	}

	template<typename T>
	lyah::vec<2, T> perpendicular(const lyah::vec<2, T>& a, T b) {
		return {a[1] * b, a[0] * b * static_cast<T>(-1)};
	}

	using lyah::abs;
	using lyah::radians;
	using lyah::rcp;

	// TODO: lyah::area
	// Returns the absolute area of a 2D polygon defined by a sequence of vertices.
	// https://stackoverflow.com/a/717367/17136841
	// https://web.archive.org/web/20111004134549/http://softsurfer.com/Archive/algorithm_0101/algorithm_0101.htm
	template<typename T>
	T area(const sequence<lyah::vec<2, T>>& unclosedVertices) {
		T area = static_cast<T>(0.0);

		// Close vertex array.
		sequence<lyah::vec<2, T>> closedVertices(unclosedVertices.count + 2);

		std::copy(unclosedVertices.begin(), unclosedVertices.end(), &closedVertices[0]);

		closedVertices[unclosedVertices.count + 0] = unclosedVertices[0];
		closedVertices[unclosedVertices.count + 1] = unclosedVertices[1];

		uint32 i = 1;

		while (i <= unclosedVertices.count) {
			area += closedVertices[i][0] * (closedVertices[i + 1][1] - closedVertices[i - 1][1]);

			i += 1;
		}

		return abs(area) * static_cast<T>(0.5);
	}

	// TODO: Refactor using sign flip for 2D perpendicular vectors.
	template<typename T>
	lyah::vec<2, T> cross2d(const lyah::vec<2, T>& a, const lyah::vec<2, T>& b) {
		const lyah::vec<3, T> a_3d = {a[0], a[1], 0.0f};
		const lyah::vec<3, T> b_3d = {b[0], b[1], 0.0f};
		const lyah::vec<3, T> r_3d = cross(a_3d, b_3d);

		return {r_3d[0], r_3d[1]};
	}

	// TODO: Refactor using sign flip for 2D perpendicular vectors.
	template<typename T>
	lyah::vec<2, T> cross2d(const lyah::vec<2, T>& a, const lyah::vec<2, T>& b, const lyah::vec<2, T>& c) {
		const lyah::vec<2, T> b_c = {b[0] * c[1], b[1] * c[0]};

		return {
			a[1] * (b_c[0] - b_c[1]),
			a[0] * (b_c[1] - b_c[0]),
		};
	}

	template<typename T>
	lyah::vec<2, T> multiplyTransform(const lyah::vec<2, T>& a, const lyah::mat<3, 3, T>& t) {
		return {
			t[0][0] * a[0] + t[1][0] * a[1] + t[2][0],
			t[0][1] * a[0] + t[1][1] * a[1] + t[2][1],
		};
	}

	// TODO: Implement in Lyah.
	template<typename T>
	lyah::mat<2, 2, T> mat2x2_rotation(T angle) {
		const T c = lyah::cos(angle);
		const T s = lyah::sin(angle);

		return {
			c, -s,
			s, c,
		};
	}

	static vec2<float32> rotate(const vec2<float32>& d, float32 r) {
		const mat2x2<float32> R = mat2x2_rotation<float32>(r);

		return d * R;
	}

	static float32 signedDistanceLinePoint(const vec2<float32>& a, const vec2<float32>& b, const vec2<float32>& p) {
		const vec2<float32> ap = p - a;
		const vec2<float32> ab = b - a;

		return parallelogramArea(ab, ap) / length(ab);
	}

	template<typename T>
	struct Interval {
		T min;
		T max;
	};

	template<typename T>
	static T overlap(const Interval<T>& intervalA, const Interval<T>& intervalB) {
		return std::max(intervalA.min - intervalB.max, intervalB.min - intervalA.max);
	}
}