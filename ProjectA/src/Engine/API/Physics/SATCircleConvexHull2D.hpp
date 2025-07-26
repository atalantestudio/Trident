#pragma once

#include "Engine/API/Geometry/ConvexHullGeometry2D.hpp"
#include "Engine/API/Physics/RigidBody.hpp"
#include "Engine/API/Physics/SAT2D.hpp"

namespace ProjectA {
	// Separating Axis Theorem algorithm implementation in 2D.
	// A has a circle geometry and B has a convex hull geometry.
	// 
	// - Dirk Gregorius - The Separating Axis Test between Convex Polyhedra
	//   https://gdcvault.com/play/1017646/Physics-for-Game-Programmers-The
	//   https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf
	// - Erin Catto - Fast and Simple Physics using Sequential Impulses
	//   https://box2d.org/files/ErinCatto_SequentialImpulses_GDC2006.pdf
	// - Tim Wheeler - 2D Collision Detection and Resolution
	//   https://timallanwheeler.com/blog/2024/08/01/2d-collision-detection-and-resolution
	// - William Bittle - SAT (Separating Axis Theorem)
	//   https://dyn4j.org/2010/01/sat
	// 
	// TODO: Basic optimizations:
	//       - Don't test the same separating axis multiple times
	//       - Don't test parallel axes (https://dyn4j.org/2010/01/sat)
	//       - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases. But what if the separation axis changes e.g. due to rotation?
	// https://media.gdcvault.com/gdc2013/slides/822403Gregorius_Dirk_TheSeparatingAxisTest.pdf
	static SAT2DOutput SATCircleConvexHull2D(const RigidBody2D& CircleBody, const RigidBody2D& ConvexHullBody) {
		SAT2DOutput output;
		output.separation = min<float32>();
		output.referenceEdge.setVertexCount(2);

		queryEdgeDirections(ConvexHullBody, CircleBody, output);

		if (output.separation > 0.0f) {
			return output;
		}

		// TODO: Since the incident body is always the circle, does the normal need to be corrected?
		output.normal = rotate(output.normal, -output.incidentBody->rotation);

		return output;
	}
}