// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Plane2D {
		vec2<float32> normal;
		float32 offset;
	};

	// TODO(lyah/signed-distance): Add to Lyah.
	/// Calculates the signed distance between a point and an infinite plane.
	inline float32 signedDistance(vec2<float32> point, Plane2D plane) {
		return dot(plane.normal, point) - plane.offset;
	}
}