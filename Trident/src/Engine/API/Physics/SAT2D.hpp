// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Physics/Feature2D.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

namespace Trident {
	struct SAT2DOutput {
		// A separation less than or equal to 0 indicates overlap between the shapes.
		float32 separation;

		// Normal of the reference edge.
		vec2<float32> normal;

		// Index of the reference edge in the geometry vertex array.
		uint32 referenceEdgeIndex;

		// Closest edge on the reference body to the incident body.
		// The vertices are already transformed.
		Feature2D referenceEdge;

		// Support point on the incident body, in the direction opposite to the reference edge's normal.
		// This is the closest point to the reference edge.
		vec2<float32> incidentSupportPoint;

		const RigidBody2DComponent* referenceBody;
		const RigidBody2DComponent* incidentBody;

		const Hull2D* referenceHull;
		const Hull2D* incidentHull;
	};

	// Separating Axis Theorem algorithm implementation in 2D.
	// 
	// TODO: Basic optimizations:
	// - Don't test the same separating axis multiple times
	// - Don't test parallel axes (https://dyn4j.org/2010/01/sat)
	// - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases.
	//   But what if the separation axis changes e.g. due to rotation?
	SAT2DOutput SAT2D(const RigidBody2DComponent& A, const RigidBody2DComponent& B, const Hull2D& hullA, const Hull2D& hullB);

	// Separating Axis Theorem algorithm implementation in 2D.
	// A has a circle geometry and B has a convex hull geometry.
	// TODO: Basic optimizations:
	// - Don't test the same separating axis multiple times
	// - Don't test parallel axes (https://dyn4j.org/2010/01/sat)
	// - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases.
	//   But what if the separation axis changes e.g. due to rotation?
	SAT2DOutput SATCircleConvexHull2D(const RigidBody2DComponent& circleBody, const RigidBody2DComponent& convexHullBody, const Hull2D& circleHull, const Hull2D& convexHullGeometry);
}