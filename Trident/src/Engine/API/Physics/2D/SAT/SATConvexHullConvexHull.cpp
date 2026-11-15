// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/SAT/SATConvexHullConvexHull.hpp"

namespace Trident {
	// TODO(ci/debugger/undefined-identifier)
	// The returned object cannot be debugged because "an unspecified error occurs".
	// See https://developercommunity.visualstudio.com/t/When-a-custom-function-returns-a-custom-/10422600.
	static SATEdgeQuery2D queryEdgeDirections(const RigidBody2D& referenceBody, const RigidBody2D& incidentBody, const Shape2D& referenceShape, const Shape2D& incidentShape) {
		SATEdgeQuery2D query;
		query.separation = min<float32>();

		const vec2<float32>* referenceVertices = referenceShape.hull->getVertices();

		const mat2x2<float32> RA = rotationAngle<mat2x2<float32>>(referenceBody.rotation);
		const mat2x2<float32> RBA = rotationAngle<mat2x2<float32>>(referenceBody.rotation - incidentBody.rotation);

		uint16 indexA = 0;

		while (indexA < referenceShape.hull->vertexCount) {
			const uint16 indexB = (indexA + 1) % referenceShape.hull->vertexCount;
			const vec2<float32> a = referenceVertices[indexA];
			const vec2<float32> b = referenceVertices[indexB];
			const vec2<float32> d = edgeNormalClockwise(a, b);
			const vec2<float32> s = vertexSupport(incidentShape, -d * RBA) * incidentBody.getWorld();

			const float32 edgeDistance = abs(signedDistanceLPAB(a, b, {0.0f, 0.0f}));
			const vec2<float32> offset = edgeDistance * d * referenceBody.getWorld();
			const float32 separation = signedDistanceLP(d * RA, offset, s);

			if (separation > query.separation) {
				query.separation = separation;
				query.axis = d;
				query.referenceEdgeIndex = indexA;
			}

			if (separation > 0.0f) {
				break;
			}

			indexA += 1;
		}

		return query;
	}

	// TODO(ci/debugger/undefined-identifier)
	// The returned object cannot be debugged because "an unspecified error occurs".
	// See https://developercommunity.visualstudio.com/t/When-a-custom-function-returns-a-custom-/10422600.
	SATConvexHullConvexHullOutput2D SATConvexHullConvexHull2D(const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, const Shape2D& shapeA, const Shape2D& shapeB) {
		DEBUG_ASSERT(shapeA.type == ShapeType2D::CONVEX_HULL);
		DEBUG_ASSERT(shapeB.type == ShapeType2D::CONVEX_HULL);

		SATConvexHullConvexHullOutput2D output;
		output.overlap = false;
		output.edgeQueryA = queryEdgeDirections(rigidBodyA, rigidBodyB, shapeA, shapeB);

		if (output.edgeQueryA.separation > 0.0f) {
			return output;
		}

		output.edgeQueryB = queryEdgeDirections(rigidBodyB, rigidBodyA, shapeB, shapeA);

		if (output.edgeQueryB.separation > 0.0f) {
			return output;
		}

		output.overlap = true;

		return output;
	}
}