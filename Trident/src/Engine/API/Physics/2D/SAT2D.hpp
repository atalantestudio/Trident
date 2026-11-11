// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/RigidBody.hpp"
#include "Engine/API/Physics/2D/Shape.hpp"

namespace Trident {
	struct SATEdgeQuery2D {
		/// The separation value between the two bodies.
		/// A negative separation indicates overlap.
		float32 separation;

		/// The separating plane on the reference body, in the local space of the incident body.
		Plane2D referencePlane;

		/// The indices of the vertices composing the separating edge.
		uint32 referenceEdgeIndexA;
		uint32 referenceEdgeIndexB;

		/// The vertices composing the separating edge.
		vec2<float32> referenceEdgeA;
		vec2<float32> referenceEdgeB;
	};

	struct SATOutput2D {
		bool overlap;
		SATEdgeQuery2D edgeQueryA;
		SATEdgeQuery2D edgeQueryB;
	};

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
	/// See https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf.
	// TODO(physics/2d/sat): Optimize:
	// - Don't test the same separating axis multiple times.
	// - Don't test parallel axes (https://dyn4j.org/2010/01/sat).
	// - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases.
	//   But what if the separation axis changes e.g. due to rotation?
	SATOutput2D SAT2D(const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, const Shape2D& shapeA, const Shape2D& shapeB);

	/// The Separating Axis Theorem algorithm in 2D.
	/// A has a circle geometry and B has a convex hull geometry.
	// TODO(physics/2d/sat): Optimize:
	// - Don't test the same separating axis multiple times.
	// - Don't test parallel axes (https://dyn4j.org/2010/01/sat).
	// - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases.
	//   But what if the separation axis changes e.g. due to rotation?
	SATCircleConvexHullOutput2D SATCircleConvexHull2D(const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, const Shape2D& shapeA, const Shape2D& shapeB);
}