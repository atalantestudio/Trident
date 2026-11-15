// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/RigidBody.hpp"
#include "Engine/API/Physics/2D/Shape/Shape.hpp"

namespace Trident {
	struct SATCircleConvexHullOutput2D {
		/// The separation between the two bodies.
		/// The shapes overlap if the separation is negative or 0.
		float32 separation;

		/// The separating axis (the normal of the reference edge).
		vec2<float32> axis;

		/// Support point on the incident body, in the direction opposite to the reference edge's normal.
		/// This is the closest point to the reference edge.
		vec2<float32> incidentSupportPoint;
	};

	/// The Separating Axis Theorem algorithm in 2D.
	// `rigidBodyA` must have the CIRCLE type and `rigidBodyB` must have the CONVEX_HULL type.
	// TODO(physics/2d/sat): Optimize:
	// - Don't test the same separating axis multiple times.
	// - Don't test parallel axes (https://dyn4j.org/2010/01/sat).
	// - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases.
	//   But what if the separation axis changes e.g. due to rotation?
	SATCircleConvexHullOutput2D SATCircleConvexHull2D(const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, const Shape2D& shapeA, const Shape2D& shapeB);
}