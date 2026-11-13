// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/SAT/SATCircleConvexHull.hpp"

namespace Trident {
	static void queryEdgeDirections(const RigidBody2DComponent& referenceBody, const RigidBody2DComponent& incidentBody, const Hull2D& referenceHull, const Hull2D& incidentHull, SATCircleConvexHullOutput2D& output) {
		const mat2x2<float32> RA = rotationAngle<mat2x2<float32>>(referenceBody.rotation);
		const mat2x2<float32> RBA = rotationAngle<mat2x2<float32>>(referenceBody.rotation - incidentBody.rotation);

		uint32 edgeIndex = 0;

		while (edgeIndex < referenceHull.edges.count()) {
			const Edge2D edge = referenceHull.edges[edgeIndex];
			const vec2<float32> a = referenceHull.vertices[edge.indexA];
			const vec2<float32> b = referenceHull.vertices[edge.indexB];
			const vec2<float32> d = edgeNormalClockwise(a, b);
			const vec2<float32> s = incidentHull.support(-d * RBA);
			const vec2<float32> sT = s * incidentBody.world;

			// The centroid is not involved when calculating the edge distance because it is applied when multiplying by the world matrix.
			const float32 edgeDistance = abs(signedDistanceLPAB(a, b, {0.0f, 0.0f}));
			const vec2<float32> offset = edgeDistance * d * referenceBody.world;

			const float32 separation = signedDistanceLP(d * RA, offset, sT);

			if (separation > output.separation) {
				output.separation = separation;
				output.axis = d;
				output.incidentSupportPoint = sT;
			}

			if (separation > 0.0f) {
				break;
			}

			edgeIndex += 1;
		}
	}

	SATCircleConvexHullOutput2D SATCircleConvexHull2D(const RigidBody2DComponent& circleBody, const RigidBody2DComponent& convexHullBody, const Hull2D& circleHull, const Hull2D& convexHullGeometry) {
		SATCircleConvexHullOutput2D output{};
		output.separation = min<float32>();

		queryEdgeDirections(convexHullBody, circleBody, convexHullGeometry, circleHull, output);

		if (output.separation > 0.0f) {
			return output;
		}

		// TODO(physics/2d/sat-circle-convex-hull): Since the incident body is always the circle, does the axis need to be corrected?
		output.axis *= rotationAngle<mat2x2<float32>>(convexHullBody.rotation);

		return output;
	}
}