#pragma once

namespace ProjectA {
	// Returns the farthest point of A in the direction d.
	inline vec2<float32> support(const Object2D& A, const vec2<float32>& d) {
		const mat3x3<float32> R = mat3x3<float32>::rotation(-A.rotation);
		const vec2<float32> col0 = {R[0][0], R[1][0]};
		const vec2<float32> col1 = {R[0][1], R[1][1]};
		const vec2<float32> dR = {dot(col0, d), dot(col1, d)};
		const vec2<float32> s = A.getGeometry().support(dR);

		// Restore the rotation of A on the found support point.
		return {
			A.transform[0][0] * s[0] + A.transform[1][0] * s[1] + A.transform[2][0],
			A.transform[0][1] * s[0] + A.transform[1][1] * s[1] + A.transform[2][1],
		};
	}
}