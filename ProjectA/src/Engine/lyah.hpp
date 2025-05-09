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
}