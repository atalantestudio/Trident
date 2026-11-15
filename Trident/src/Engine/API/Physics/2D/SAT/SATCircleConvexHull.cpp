// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Base.hpp"
#include "Engine/API/Physics/2D/SAT/SATCircleConvexHull.hpp"

namespace Trident {
	// `referenceBody` is the convex hull, `incidentBody` is the circle.
	static void queryEdgeDirections(const RigidBody2D& referenceBody, const RigidBody2D& incidentBody, const Shape2D& referenceShape, SATCircleConvexHullOutput2D& output) {
		const vec2<float32>* vertices = referenceShape.hull->getVertices();

		uint32 vertexIndex = 0;

		while (vertexIndex < referenceShape.hull->vertexCount) {
			const vec2<float32> a = vertices[vertexIndex];
			const vec2<float32> b = vertices[(vertexIndex + 1) % referenceShape.hull->vertexCount];
			const vec2<float32> d = edgeNormalClockwise(a, b);

			// TODO(physics/2d/sat/circle-convex-hull): Optimize.
			const vec2<float32> sT = incidentBody.transform.position;

			// The centroid is not involved when calculating the edge distance because it is applied when multiplying by the world matrix.
			const float32 edgeDistance = abs(signedDistanceLPAB(a, b, {0.0f, 0.0f}));
			const vec2<float32> offset = edgeDistance * d * referenceBody.world;

			const float32 separation = signedDistanceLP(d * referenceBody.rotation, offset, sT);

			if (separation > output.separation) {
				output.separation = separation;
				output.axis = d;
				output.incidentSupportPoint = sT;
			}

			if (separation > 0.0f) {
				break;
			}

			vertexIndex += 1;
		}
	}

	SATCircleConvexHullOutput2D SATCircleConvexHull2D(const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, const Shape2D& shapeA, const Shape2D& shapeB) {
		SATCircleConvexHullOutput2D output{};
		output.separation = min<float32>();

		queryEdgeDirections(rigidBodyB, rigidBodyA, shapeB, output);

		if (output.separation > 0.0f) {
			return output;
		}

		// TODO(physics/2d/sat/circle-convex-hull): Since the incident body is always the circle, does the axis need to be corrected?
		output.axis = output.axis * rigidBodyB.rotation;

		return output;
	}
}