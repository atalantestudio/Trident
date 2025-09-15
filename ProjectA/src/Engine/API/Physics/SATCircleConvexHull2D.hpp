#pragma once

#include "Engine/API/Geometry/ConvexHullGeometry2D.hpp"
#include "Engine/API/Physics/RigidBody.hpp"
#include "Engine/API/Physics/SAT2D.hpp"

namespace ProjectA {
	// Separating Axis Theorem algorithm implementation in 2D.
	// A has a circle geometry and B has a convex hull geometry.
	// TODO: Basic optimizations:
	//       - Don't test the same separating axis multiple times
	//       - Don't test parallel axes (https://dyn4j.org/2010/01/sat)
	//       - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases. But what if the separation axis changes e.g. due to rotation?
	static SAT2DOutput SATCircleConvexHull2D(const RigidBody2D& CircleBody, const RigidBody2D& ConvexHullBody) {
		SAT2DOutput output;
		output.separation = min<float32>();
		output.referenceEdge.setVertexCount(2);

		queryEdgeDirections(ConvexHullBody, CircleBody, output);

		if (output.separation > 0.0f) {
			return output;
		}

		// TODO: Since the incident body is always the circle, does the normal need to be corrected?
		output.normal = output.normal * mat2x2<float32>::rotation(-output.incidentBody->rotation);

		return output;
	}
}