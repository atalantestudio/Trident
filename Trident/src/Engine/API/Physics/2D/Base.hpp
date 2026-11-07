// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/Plane.hpp"

namespace Trident {
	struct PhysicsSimulation2D;
	struct RigidBody2D;
	struct Shape2D;

	typedef uint32 RigidBodyID2D;
	typedef uint32 ShapeID2D;

	// TODO(lyah/signed-distance): Add to Lyah.
	/// Calculates the signed distance between a point and an infinite plane.
	inline float32 signedDistance(vec2<float32> point, Plane2D plane) {
		return dot(point, plane.normal) - plane.offset;
	}

	// TODO(lyah/signed-distance): Add to Lyah.
	/// Calculates the signed distance between a point and an infinite line represented by a normal and a vector offset.
	inline float32 signedDistanceLP(vec2<float32> normal, vec2<float32> offset, vec2<float32> point) {
		return dot(normal, point - offset);
	}

	inline vec2<float32> edgeNormalClockwise(vec2<float32> a, vec2<float32> b) {
		return normalized(perpendicularLeft(b - a));
	}
}