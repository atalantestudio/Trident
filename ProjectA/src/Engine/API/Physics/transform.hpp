#pragma once

namespace ProjectA {
	static vec2<float32> multiplyTransform(const vec2<float32>& a, const mat3x3<float32>& T) {
		return {
			T[0][0] * a[0] + T[1][0] * a[1] + T[2][0],
			T[0][1] * a[0] + T[1][1] * a[1] + T[2][1],
		};
	}
}