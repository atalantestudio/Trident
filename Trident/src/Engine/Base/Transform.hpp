// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Plane3D.hpp"

namespace Trident {
	struct Transform3D {
		vec3<float32> position;
		quat<float32> orientation;
	};

	/// Returns the product of `TA` and `TB`.
	inline Transform3D operator*(const Transform3D& TA, Transform3D TB) {
		TB.orientation = TA.orientation * TB.orientation;
		TB.position = TB.position * TA.orientation + TA.position;

		return TB;
	}

	/// Returns the product of `p` by `A`.
	inline vec3<float32> operator*(vec3<float32> p, const Transform3D& T) {
		return p * T.orientation + T.position;
	}

	inline vec3<float32>& operator*=(vec3<float32>& p, const Transform3D& T) {
		return p = p * T;
	}

	inline Plane3D operator*(Plane3D plane, const Transform3D& A) {
		plane.normal *= A.orientation;
		plane.distance += dot(plane.normal, A.position);
		plane.edgeIndexRange = plane.edgeIndexRange;

		return plane;
	}

	/// Returns the inverse of `A`.
	inline Transform3D inverse(Transform3D A) {
		A.orientation = conjugate(A.orientation);
		A.position = -A.position * A.orientation;

		return A;
	}
}