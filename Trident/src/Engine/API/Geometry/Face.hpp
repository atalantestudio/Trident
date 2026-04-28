// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Face3D {
		svec3<float32> normal;
		float32 offset;

		// TODO: The count should be 8-bits, as a face cannot be referred by more than 255 edges.
		range<uint16, uint16> edgeIndexRange;
	};

	/// Calculates and returns the signed distance between a point and a plane in world space.
	inline float32 signedDistance(Face3D f, vec3<float32> p) {
		return dot(vec3<float32>(f.normal), p) - f.offset;
	}
}