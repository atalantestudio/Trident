#pragma once

#include <lyah/lyah.hpp>

namespace lyah {
	// TODO: Use static arrays instead of class as polyfills.

	// vec<2, sint32> polyfill.
	template<>
	struct lyah::vec<2, std::int32_t> {
		public:
			std::int32_t operator [](std::size_t index) const noexcept {
				assert(index < 2);

				return m[index];
			}

			std::int32_t& operator [](std::size_t index) noexcept {
				assert(index < 2);

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
				assert(index < 3);

				return m[index];
			}

			std::int32_t& operator [](std::size_t index) noexcept {
				assert(index < 3);

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
				assert(index < 2);

				return m[index];
			}

			std::uint32_t& operator [](std::size_t index) noexcept {
				assert(index < 2);

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
				assert(index < 3);

				return m[index];
			}

			std::uint32_t& operator [](std::size_t index) noexcept {
				assert(index < 3);

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
				assert(index < 4);

				return m[index];
			}

			std::uint32_t& operator [](std::size_t index) noexcept {
				assert(index < 4);

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
			// TODO: Assert.
			return *pointer_const_cast<const T*>(this)[index];
		}

		T& operator [](std::size_t index) {
			// TODO: Assert.
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
			// TODO: Assert.
			return *pointer_const_cast<const T*>(this)[index];
		}

		T& operator [](std::size_t index) {
			// TODO: Assert.
			return *pointer_const<T*>(this)[index];
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

	// TODO: Squared length in Lyah.
	template<std::size_t C, typename T>
	T lengthSq(const lyah::vec<C, T>& a) {
		return lyah::dot(a, a);
	}

	// TODO: Squared distance in Lyah.
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
			DEBUG_ASSERT(index < count);

			return m[index];
		}
	};

	template<typename T>
	using sim2 = sim<2, T>;

	template<std::size_t C, typename T>
	T area(const lyah::vec<C, T>& a, const lyah::vec<C, T>& b) {
		return a[0] * b[1] - a[1] * b[0];
	}

	template<typename T>
	lyah::vec<2, T> perpendicular(const lyah::vec<2, T>& a, T b) {
		return {b * a[1], static_cast<T>(-1) * b * a[0]};
	}

	using lyah::abs;
}