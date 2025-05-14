#pragma once

namespace ProjectA {
	static vec2<float32> cancelRotation(const Object2D& A, const vec2<float32>& d) {
		const mat3x3<float32> R = mat3x3<float32>::rotation(-A.rotation);
		const vec2<float32> R0 = {R[0][0], R[1][0]};
		const vec2<float32> R1 = {R[0][1], R[1][1]};

		return {dot(R0, d), dot(R1, d)};
	}

	// Restore the rotation of A onto the support point.
	static vec2<float32> transformSupport(const Object2D& A, uint32 supportIndex) {
		const vec2<float32>& a = A.getGeometry().vertices[supportIndex];

		return {
			A.transform[0][0] * a[0] + A.transform[1][0] * a[1] + A.transform[2][0],
			A.transform[0][1] * a[0] + A.transform[1][1] * a[1] + A.transform[2][1],
		};
	}

	// Returns the index of the farthest point of A in the direction d.
	inline uint32 supportIndex(const Object2D& A, const vec2<float32>& d) {
		const vec2<float32> dR = cancelRotation(A, d);

		return A.getGeometry().support(dR);
	}

	// Returns the farthest point of A in the direction d.
	inline vec2<float32> support(const Object2D& A, const vec2<float32>& d) {
		const uint32 i = supportIndex(A, d);

		return transformSupport(A, i);
	}
}