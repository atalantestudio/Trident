// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Base.hpp"
#include "Engine/API/Physics/2D/SAT/SATConvexHullConvexHull.hpp"

namespace Trident {
	/* static SATEdgeQuery2D queryEdgeDirections(const RigidBody2D& referenceBody, const RigidBody2D& incidentBody, const Shape2D& referenceShape, const Shape2D& incidentShape) {
		SATEdgeQuery2D query{};
		query.separation = min<float32>();

		const Transform2D transform = referenceBody.transform * inverse(incidentBody.transform);
		const vec2<float32>* vertices = referenceShape.hull->getVertices();
		const uint16 vertexCount = referenceShape.hull->vertexCount;

		uint16 indexA = 0;

		while (indexA < vertexCount) {
			const uint16 indexB = (indexA + 1) % vertexCount;

			const vec2<float32> a = vertices[indexA] * transform;
			const vec2<float32> b = vertices[indexB] * transform;

			Plane2D plane;
			plane.normal = edgeNormalClockwise(a, b);
			plane.offset = dot(plane.normal, a);

			const vec2<float32> s = vertexSupport(incidentShape, -plane.normal);
			const float32 separation = signedDistance(s, plane);

			if (separation > query.separation) {
				query.separation = separation;
				query.referencePlane = plane;
				query.referenceEdgeIndexA = indexA;
				query.referenceEdgeIndexB = indexB;
			}

			if (separation > 0.0f) {
				break;
			}

			indexA += 1;
		}

		return query;
	} */
	static SATEdgeQuery2D queryEdgeDirections(const Shape2D& referenceShape, const Shape2D& incidentShape, const RigidBody2D& referenceBody, const RigidBody2D& incidentBody) {
		SATEdgeQuery2D query;
		query.separation = min<float32>();

		const vec2<float32>* vertices = referenceShape.hull->getVertices();

		const mat2x2<float32> RA = rotationAngle<mat2x2<float32>>(referenceBody.rotation);
		const mat2x2<float32> RBA = rotationAngle<mat2x2<float32>>(referenceBody.rotation - incidentBody.rotation);

		uint16 indexA = 0;

		while (indexA < referenceShape.hull->vertexCount) {
			const uint16 indexB = (indexA + 1) % referenceShape.hull->vertexCount;
			const vec2<float32> a = vertices[indexA];
			const vec2<float32> b = vertices[indexB];
			const vec2<float32> d = edgeNormalClockwise(a, b);
			const vec2<float32> s = vertexSupport(incidentShape, -d * RBA);

			const float32 edgeDistance = abs(signedDistanceLPAB(a, b, {0.0f, 0.0f}));
			const vec2<float32> offset = edgeDistance * d * referenceBody.world;
			const float32 separation = signedDistanceLP(d * RA, offset, s * incidentBody.world);

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
	SATConvexHullConvexHullOutput2D SATConvexHullConvexHull2D(const Shape2D& shapeA, const Shape2D& shapeB, const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB) {
		DEBUG_ASSERT(shapeA.type == ShapeType2D::CONVEX_HULL);
		DEBUG_ASSERT(shapeB.type == ShapeType2D::CONVEX_HULL);

		SATConvexHullConvexHullOutput2D output;
		output.overlap = false;
		output.edgeQueryA = queryEdgeDirections(shapeA, shapeB, rigidBodyA, rigidBodyB);

		if (output.edgeQueryA.separation > 0.0f) {
			return output;
		}

		output.edgeQueryB = queryEdgeDirections(shapeB, shapeA, rigidBodyB, rigidBodyA);

		if (output.edgeQueryB.separation > 0.0f) {
			return output;
		}

		output.overlap = true;

		return output;
	}
}