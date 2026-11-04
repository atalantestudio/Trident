// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Plane3D {
		vec3<float32> normal;
		float32 distance;

		// TODO(base/plane): The count should be 8-bits, as a face cannot be referred to by more than 255 edges.
		range<uint16, uint16> edgeIndexRange;
	};

	/// Calculates and returns the 3D signed distance between a plane `P` and a point `p`.
	inline float32 signedDistance(const Plane3D& P, vec3<float32> p) {
		return dot(P.normal, p) - P.distance;
	}
}