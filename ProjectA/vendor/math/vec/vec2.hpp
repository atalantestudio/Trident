#pragma once

#include <utility>

namespace ProjectA {
	template<typename T>
	struct vec2 {
		T x, y;

		vec2(T x, T y);
	};

	template struct vec2<float>;
}