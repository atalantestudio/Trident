// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Base.hpp"
#include "Engine/API/Physics/2D/SAT2D.hpp"

namespace Trident {
	static void queryEdgeDirections(const RigidBody2DComponent& referenceBody, const RigidBody2DComponent& incidentBody, const Hull2D& referenceHull, const Hull2D& incidentHull, SAT2DOutput& output) {
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
				output.referenceEdgeIndex = edgeIndex;
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

	// TODO(ci/debugger/undefined-identifier)
	// `output` cannot be debugged because "an unspecified error occurs".
	// See https://developercommunity.visualstudio.com/t/When-a-custom-function-returns-a-custom-/10422600.
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

	static mat3x3<float32> world(const RigidBody2D& rigidBody) {
		return (
			translation<mat3x3<float32>>(-rigidBody.centroid) *
			rotationAngle<mat3x3<float32>>(rigidBody.rotation) *
			translation<mat3x3<float32>>(rigidBody.position)
		);
	}

	// `referenceBody` is the convex hull, `incidentBody` is the circle.
	static void queryEdgeDirectionsCircleConvexHull(const RigidBody2D& referenceBody, const RigidBody2D& incidentBody, const Shape2D& referenceShape, const Shape2D& incidentShape, SATCircleConvexHullOutput2D& output) {
		const vec2<float32>* vertices = pointer_cast<const vec2<float32>*>(pointer_cast<const uint8*>(referenceShape.hull) + referenceShape.hull->vertexOffset);
		const mat2x2<float32> RA = rotationAngle<mat2x2<float32>>(referenceBody.rotation);

		uint32 vertexIndex = 0;

		while (vertexIndex < referenceShape.hull->vertexCount) {
			const vec2<float32> a = vertices[vertexIndex];
			const vec2<float32> b = vertices[(vertexIndex + 1) % referenceShape.hull->vertexCount];
			const vec2<float32> d = edgeNormalClockwise(a, b);

			// TODO(physics/2d/sat): Optimize.
			const vec2<float32> sT = incidentBody.position;

			// The centroid is not involved when calculating the edge distance because it is applied when multiplying by the world matrix.
			const float32 edgeDistance = abs(signedDistanceLPAB(a, b, {0.0f, 0.0f}));
			const vec2<float32> offset = edgeDistance * d * world(referenceBody);

			const float32 separation = signedDistanceLP(d * RA, offset, sT);

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
		output.axis *= rotationAngle<mat2x2<float32>>(rigidBodyB.rotation);

		return output;
	}
}