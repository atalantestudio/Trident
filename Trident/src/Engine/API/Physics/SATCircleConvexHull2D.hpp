// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/RigidBody2D.hpp"
#include "Engine/API/Physics/SAT2D.hpp"

namespace Trident {
	// Separating Axis Theorem algorithm implementation in 2D.
	// A has a circle geometry and B has a convex hull geometry.
	// TODO: Basic optimizations:
	// - Don't test the same separating axis multiple times
	// - Don't test parallel axes (https://dyn4j.org/2010/01/sat)
	// - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases.
	//   But what if the separation axis changes e.g. due to rotation?
	static SAT2DOutput SATCircleConvexHull2D(const RigidBody2D& circleBody, const RigidBody2D& convexHullBody, const Hull2D& circleHull, const Hull2D& convexHullGeometry) {
		SAT2DOutput output{};
		output.separation = min<float32>();
		output.referenceEdge.vertexCount = 2;

		queryEdgeDirections(convexHullBody, circleBody, convexHullGeometry, circleHull, output);

		if (output.separation > 0.0f) {
			return output;
		}

		// TODO: Since the incident body is always the circle, does the normal need to be corrected?
		output.normal = output.normal * mat2x2<float32>::rotation(-output.incidentBody->rotation);

		return output;
	}
}