// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	typedef uint32 RigidBodyID2D;
	typedef uint32 ShapeID2D;

	// TODO(lyah/signed-distance): Add to Lyah.
	/// Calculates the signed distance between a point and an infinite line represented by a normal and a vector offset.
	inline float32 signedDistanceLP(vec2<float32> normal, vec2<float32> offset, vec2<float32> point) {
		return dot(normal, point - offset);
	}

	inline vec2<float32> edgeNormalClockwise(vec2<float32> a, vec2<float32> b) {
		return normalized(perpendicularLeft(b - a));
	}
}