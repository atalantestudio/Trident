// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Physics/2D/RigidBody.hpp"

namespace Trident {
	struct SATEdgeQuery2D {
		/// The separation value between the two bodies.
		/// A negative separation indicates overlap.
		float32 separation;

		/// The separating plane on the reference body, in the local space of the incident body.
		// Plane2D referencePlane;
		vec2<float32> axis;

		/// The indices of the vertices composing the separating edge.
		// uint32 referenceEdgeIndexA;
		// uint32 referenceEdgeIndexB;
		uint32 referenceEdgeIndex;
	};

	struct SATConvexHullConvexHullOutput2D {
		bool overlap;
		SATEdgeQuery2D edgeQueryA;
		SATEdgeQuery2D edgeQueryB;
	};

	/// The Separating Axis Theorem algorithm in 2D.
	/// See https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf.
	// TODO(physics/2d/sat): Optimize:
	// - Don't test the same separating axis multiple times.
	// - Don't test parallel axes (https://dyn4j.org/2010/01/sat).
	// - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases.
	//   But what if the separation axis changes e.g. due to rotation?
	SATConvexHullConvexHullOutput2D SATConvexHullConvexHull2D(const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, const Hull2D& shapeA, const Hull2D& shapeB);
}