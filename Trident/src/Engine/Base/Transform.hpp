// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Plane3D.hpp"

namespace Trident {
	struct Transform3D {
		vec3<float32> position;
		quat<float32> orientation;
	};

	/// Returns the product of the transforms `A` and `B`.
	inline Transform3D operator*(const Transform3D& A, Transform3D B) {
		B.position = B.position * A.orientation + A.position;

		// NOTE: Both orders work.
		B.orientation = A.orientation * B.orientation;

		return B;
	}

	/// Returns the product of `p` by the transform `A`.
	/* inline vec3<float32> operator*(vec3<float32> p, const Transform3D& A) {
		return p * A.orientation + A.position;
	} */

	inline Plane3D operator*(Plane3D plane, const Transform3D& A) {
		plane.normal = svec3<float32>(vec3<float32>(plane.normal) * A.orientation);
		plane.distance = dot(vec3<float32>(plane.normal), A.position) + plane.distance;

		return plane;
	}

	/// Returns the inverse transform of `A`.
	inline Transform3D inverse(Transform3D A) {
		A.position = -A.position;
		A.orientation = conjugate(A.orientation);

		return A;
	}
}