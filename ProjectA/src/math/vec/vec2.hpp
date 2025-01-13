#pragma once

#include <utility>

namespace ProjectA {
	template<typename T>
	struct vec2 {
		T x, y;

		vec2() = default;
		vec2(T x, T y) : x(x), y(y) {}

		const T operator [](unsigned int index) const {
			if (index == 0) {
				return x;
			}
			else {
				return y;
			}
		}

		T& operator [](unsigned int index) {
			if (index == 0) {
				return x;
			}
			else {
				return y;
			}
		}
	};

	template struct vec2<float>;
}