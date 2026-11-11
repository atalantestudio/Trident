// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Base.hpp"
#include "Engine/API/Physics/2D/SAT2D.hpp"

namespace Trident {
	static SATEdgeQuery2D queryEdgeDirections(const RigidBody2D& referenceBody, const RigidBody2D& incidentBody, const Shape2D& referenceShape, const Shape2D& incidentShape) {
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
				query.referenceEdgeA = a;
				query.referenceEdgeB = b;
			}

			if (separation > 0.0f) {
				break;
			}

			indexA += 1;
		}

		return query;
	}

	// TODO(ci/debugger/undefined-identifier)
	// `output` cannot be debugged because "an unspecified error occurs".
	// See https://developercommunity.visualstudio.com/t/When-a-custom-function-returns-a-custom-/10422600.
	SATOutput2D SAT2D(const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, const Shape2D& shapeA, const Shape2D& shapeB) {
		DEBUG_ASSERT(shapeA.type == ShapeType2D::CONVEX_HULL);
		DEBUG_ASSERT(shapeB.type == ShapeType2D::CONVEX_HULL);

		SATOutput2D output{};
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

	// `referenceBody` is the convex hull, `incidentBody` is the circle.
	static void queryEdgeDirectionsCircleConvexHull(const RigidBody2D& referenceBody, const RigidBody2D& incidentBody, const Shape2D& referenceShape, const Shape2D& incidentShape, SATCircleConvexHullOutput2D& output) {
		const vec2<float32>* vertices = referenceShape.hull->getVertices();

		uint32 vertexIndex = 0;

		while (vertexIndex < referenceShape.hull->vertexCount) {
			const vec2<float32> a = vertices[vertexIndex];
			const vec2<float32> b = vertices[(vertexIndex + 1) % referenceShape.hull->vertexCount];
			const vec2<float32> d = edgeNormalClockwise(a, b);

			// TODO(physics/2d/sat): Optimize.
			const vec2<float32> sT = incidentBody.transform.position;

			// The centroid is not involved when calculating the edge distance because it is applied when multiplying by the world matrix.
			const float32 edgeDistance = abs(signedDistanceLPAB(a, b, {0.0f, 0.0f}));
			const vec2<float32> offset = edgeDistance * d * referenceBody.transform;

			const float32 separation = signedDistanceLP(d * referenceBody.transform.rotation, offset, sT);

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

		queryEdgeDirectionsCircleConvexHull(rigidBodyB, rigidBodyA, shapeB, shapeA, output);

		if (output.separation > 0.0f) {
			return output;
		}

		// TODO(physics/2d/sat-circle-convex-hull): Since the incident body is always the circle, does the axis need to be corrected?
		output.axis = output.axis * rigidBodyB.transform.rotation;

		return output;
	}
}