// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/SAT2D.hpp"

namespace Trident {
	static void queryEdgeDirections(const RigidBody2DComponent& referenceBody, const RigidBody2DComponent& incidentBody, const Hull2D& referenceHull, const Hull2D& incidentHull, SAT2DOutput& output) {
		const mat2x2<float32> R = mat2x2<float32>::rotation(incidentBody.rotation - referenceBody.rotation);

		uint32 edgeIndex = 0;

		while (edgeIndex < referenceHull.edges.count()) {
			const Edge edge = referenceHull.edges[edgeIndex];
			const vec2<float32> a = referenceHull.vertices[edge.indexA];
			const vec2<float32> b = referenceHull.vertices[edge.indexB];
			const vec2<float32> d = edgeNormalClockwise(a, b);
			const vec2<float32> s = incidentHull.support(-d * R);
			const vec2<float32> aT = multiplyTransform(a, referenceBody.world);
			const vec2<float32> bT = multiplyTransform(b, referenceBody.world);
			const vec2<float32> sT = multiplyTransform(s, incidentBody.world);
			const float32 separation = signedDistanceLinePoint(aT, bT, sT);

			if (separation > output.separation) {
				output.separation = separation;
				output.normal = d;
				output.referenceEdgeIndex = edgeIndex;
				output.referenceEdge.edge[0] = aT;
				output.referenceEdge.edge[1] = bT;
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

	// Separating Axis Theorem algorithm implementation in 2D.
	// 
	// TODO: Basic optimizations:
	// - Don't test the same separating axis multiple times
	// - Don't test parallel axes (https://dyn4j.org/2010/01/sat)
	// - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases.
	//   But what if the separation axis changes e.g. due to rotation?
	SAT2DOutput SAT2D(const RigidBody2DComponent& A, const RigidBody2DComponent& B, const Hull2D& hullA, const Hull2D& hullB) {
		SAT2DOutput output{};
		output.separation = min<float32>();
		output.referenceEdge.vertexCount = 2;

		queryEdgeDirections(A, B, hullA, hullB, output);

		if (output.separation > 0.0f) {
			return output;
		}

		queryEdgeDirections(B, A, hullB, hullA, output);

		if (output.separation > 0.0f) {
			return output;
		}

		output.normal *= mat2x2<float32>::rotation(-output.referenceBody->rotation);

		return output;
	}

	SAT2DOutput SATCircleConvexHull2D(const RigidBody2DComponent& circleBody, const RigidBody2DComponent& convexHullBody, const Hull2D& circleHull, const Hull2D& convexHullGeometry) {
		SAT2DOutput output{};
		output.separation = min<float32>();
		output.referenceEdge.vertexCount = 2;

		queryEdgeDirections(convexHullBody, circleBody, convexHullGeometry, circleHull, output);

		if (output.separation > 0.0f) {
			return output;
		}

		// TODO: Since the incident body is always the circle, does the normal need to be corrected?
		output.normal *= mat2x2<float32>::rotation(-output.incidentBody->rotation);

		return output;
	}
}