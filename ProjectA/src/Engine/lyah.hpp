#pragma once

// TODO: Move into Lyah.
#include <cstdint>

#include <lyah/lyah.hpp>

namespace ProjectA {
	// TODO: Expose Lyah methods.

	template<std::size_t C, typename T>
	struct svec;

	template<typename T>
	struct svec<2, T> {
		T x;
		T y;

		constexpr svec() :
			x(static_cast<T>(0)), y(static_cast<T>(0))
		{}

		constexpr svec(T x, T y) :
			x(x), y(y)
		{}
	};

	template<typename T>
	struct svec<3, T> {
		T x;
		T y;
		T z;

		constexpr svec() :
			x(static_cast<T>(0)), y(static_cast<T>(0)), y(static_cast<T>(0))
		{}

		constexpr svec(T x, T y, T z) :
			x(x), y(y), z(z)
		{}
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

	template<typename T>
	using svec3 = svec<3, T>;

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

		sim<2, T>::sim() : count(0) {}

		sim<2, T>::sim(const lyah::vec<2, T>& a) : m{a}, count(1) {}

		sim<2, T>::sim(const lyah::vec<2, T>& a, const lyah::vec<2, T>& b) : m{a, b}, count(2) {}

		const lyah::vec<2, T>& operator [](std::uint8_t index) const {
			DEBUG_ASSERT(index < count);

			return m[index];
		}
	};

	template<std::size_t C, typename T>
	void operator +=(sim<C, T>& s, const lyah::vec<C, T>& a) {
		ENGINE_ASSERT(s.count <= C + 1, "Simplex dimension overflow.");

		s.m[s.count] = a;
		s.count += 1;
	}

	template<typename T>
	using sim2 = sim<2, T>;
}