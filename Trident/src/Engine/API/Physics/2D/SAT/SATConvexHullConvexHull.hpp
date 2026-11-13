// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

namespace Trident {
	struct SATConvexHullConvexHullOutput2D {
		/// The separation between the two bodies.
		/// The shapes overlap if the separation is negative or 0.
		float32 separation;

		// The separating axis (the normal of the reference edge).
		vec2<float32> axis;

		// Index of the reference edge in the geometry vertex array.
		uint32 referenceEdgeIndex;

		const RigidBody2DComponent* referenceBody;
		const RigidBody2DComponent* incidentBody;

		const Hull2D* referenceHull;
		const Hull2D* incidentHull;
	};

	/// The Separating Axis Theorem algorithm in 2D.
	/// See https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf.
	// TODO(physics/2d/sat): Optimize:
	// - Don't test the same separating axis multiple times.
	// - Don't test parallel axes (https://dyn4j.org/2010/01/sat).
	// - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases.
	//   But what if the separation axis changes e.g. due to rotation?
	SATConvexHullConvexHullOutput2D SATConvexHullConvexHull2D(const RigidBody2DComponent& A, const RigidBody2DComponent& B, const Hull2D& hullA, const Hull2D& hullB);
}