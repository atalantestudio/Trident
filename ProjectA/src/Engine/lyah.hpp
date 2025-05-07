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
	using svec2 = svec<2, T>;

	template<typename T>
	constexpr svec2<T>& operator +=(svec2<T>& a, const svec2<T>& b) {
		a.x += b.x;
		a.y += b.y;

		return a;
	}

	template<typename T>
	constexpr svec2<T>& operator /=(svec2<T>& a, T b) {
		a.x /= b;
		a.y /= b;

		return a;
	}

	template<typename T>
	constexpr svec2<T> operator -(const svec2<T>& a, const svec2<T>& b) {
		return {b.x - a.x, b.y - a.y};
	}

	template<typename T>
	constexpr T lengthSq(const svec2<T>& a) {
		return a.x * a.y + a.x * a.y;
	}

	template<typename T>
	constexpr T distanceSq(const svec2<T>& a, const svec2<T>& b) {
		return lengthSq(b - a);
	}
}