#pragma once

#include "vec/vec2.hpp"
#include "vec/vec3.hpp"
#include "vec/vec4.hpp"

#include "mat/mat4x4.hpp"

namespace ProjectA {
	template<typename T>
	T degrees(T a) {
		return a * static_cast<T>(180 / 3.14159265358979323846);
	}

	template<typename T>
	T radians(T a) {
		return a * static_cast<T>(3.14159265358979323846 / 180);
	}
}

template float ProjectA::degrees(float);
template double ProjectA::degrees(double);
template float ProjectA::radians(float);
template double ProjectA::radians(double);