// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Plane3D {
		vec3<float32> normal;
		float32 distance;

		// TODO: The count should be 8-bits, as a face cannot be referred to by more than 255 edges.
		range<uint16, uint16> edgeIndexRange;
	};

	/// Calculates and returns the signed distance between a point and a plane in world space.
	inline float32 signedDistance(Plane3D plane, vec3<float32> p) {
		return dot(plane.normal, p) - plane.distance;
	}
}