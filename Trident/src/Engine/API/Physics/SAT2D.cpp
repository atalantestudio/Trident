// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/SAT2D.hpp"

namespace Trident {
	static void queryEdgeDirections(const RigidBody2DComponent& referenceBody, const RigidBody2DComponent& incidentBody, const Hull2D& referenceHull, const Hull2D& incidentHull, SAT2DOutput& output) {
		const mat2x2<float32> RBA = rotationAngle<mat2x2<float32>>(referenceBody.rotation - incidentBody.rotation);
		const mat2x2<float32> RA = rotationAngle<mat2x2<float32>>(referenceBody.rotation);

		uint32 edgeIndex = 0;

		while (edgeIndex < referenceHull.edges.count()) {
			const Edge2D edge = referenceHull.edges[edgeIndex];
			const vec2<float32> a = referenceHull.vertices[edge.indexA];
			const vec2<float32> b = referenceHull.vertices[edge.indexB];
			const vec2<float32> d = edgeNormalClockwise(a, b);
			const vec2<float32> s = incidentHull.support(-d * RBA);
			const vec2<float32> sT = multiplyTransform(s, incidentBody.world);

			// The centroid is not involved when calculating the edge distance because it is applied when multiplying by the world matrix.
			const float32 edgeDistance = abs(signedDistanceLPAB(a, b, {0.0f, 0.0f}));
			const vec2<float32> offset = multiplyTransform(edgeDistance * d, referenceBody.world);

			const float32 separation = signedDistanceLP(d * RA, offset, sT);

			if (separation > output.separation) {
				output.separation = separation;
				output.axis = d;
				output.referenceEdgeIndex = edgeIndex;
				output.incidentSupportPoint = sT;
				output.referenceBody = &referenceBody;
				output.incidentBody = &incidentBody;
				output.referenceHull = &referenceHull;
				output.incidentHull = &incidentHull;
			}

			if (separation > 0.0f) {
				break;
			}

			edgeIndex += 1;
		}
	}

	SAT2DOutput SAT2D(const RigidBody2DComponent& A, const RigidBody2DComponent& B, const Hull2D& hullA, const Hull2D& hullB) {
		SAT2DOutput output{};
		output.separation = min<float32>();

		queryEdgeDirections(A, B, hullA, hullB, output);

		if (output.separation > 0.0f) {
			return output;
		}

		queryEdgeDirections(B, A, hullB, hullA, output);

		if (output.separation > 0.0f) {
			return output;
		}

		output.axis *= rotationAngle<mat2x2<float32>>(output.referenceBody->rotation);

		return output;
	}

	SAT2DOutput SATCircleConvexHull2D(const RigidBody2DComponent& circleBody, const RigidBody2DComponent& convexHullBody, const Hull2D& circleHull, const Hull2D& convexHullGeometry) {
		SAT2DOutput output{};
		output.separation = min<float32>();

		queryEdgeDirections(convexHullBody, circleBody, convexHullGeometry, circleHull, output);

		if (output.separation > 0.0f) {
			return output;
		}

		// TODO(physics/2d/sat-circle-convex-hull): Since the incident body is always the circle, does the axis need to be corrected?
		output.axis *= rotationAngle<mat2x2<float32>>(output.referenceBody->rotation);

		return output;
	}
}