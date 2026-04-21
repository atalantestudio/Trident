// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Plane3D {
		/// Normal in local space.
		vec3<float32> localNormal;

		/// Distance from the origin in local space.
		float32 localDistance;

		/// Normal in world space.
		vec3<float32> worldNormal;

		/// Distance from the origin in world space.
		float32 worldDistance;
	};

	/// Calculates and returns the signed distance between a point and a plane in world space.
	inline float32 signedDistanceWorld(const Plane3D& P, vec3<float32> p) {
		return dot(P.worldNormal, p) - P.worldDistance;
	}
}