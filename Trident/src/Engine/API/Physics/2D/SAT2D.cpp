// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Base.hpp"
#include "Engine/API/Physics/2D/SAT2D.hpp"

namespace Trident {
	static mat3x3<float32> world(const RigidBody2D& rigidBody) {
		return (
			translation<mat3x3<float32>>(-rigidBody.centroid) *
			rotationAngle<mat3x3<float32>>(rigidBody.rotation) *
			translation<mat3x3<float32>>(rigidBody.position)
		);
	}

	static vec2<float32> support(const Shape2D& shape, vec2<float32> d) {
		const vec2<float32>* vertices = pointer_cast<const vec2<float32>*>(pointer_cast<const uint8*>(shape.hull) + shape.hull->vertexOffset);

		float32 maxValue = dot(vertices[0], d);
		uint32 maxVertexIndex = 0;
		uint32 vertexIndex = 1;

		while (vertexIndex < shape.hull->vertexCount) {
			const float32 value = dot(vertices[vertexIndex], d);

			if (value > maxValue) {
				maxValue = value;
				maxVertexIndex = vertexIndex;
			}

			vertexIndex += 1;
		}

		return vertices[maxVertexIndex];
	}

	static void queryEdgeDirections(const RigidBody2D& referenceBody, const RigidBody2D& incidentBody, const Shape2D& referenceShape, const Shape2D& incidentShape, SAT2DOutput& output) {
		const vec2<float32>* vertices = pointer_cast<const vec2<float32>*>(pointer_cast<const uint8*>(referenceShape.hull) + referenceShape.hull->vertexOffset);
		const mat2x2<float32> RA = rotationAngle<mat2x2<float32>>(referenceBody.rotation);
		const mat2x2<float32> RBA = rotationAngle<mat2x2<float32>>(referenceBody.rotation - incidentBody.rotation);

		uint32 vertexIndex = 0;

		while (vertexIndex < referenceShape.hull->vertexCount) {
			const vec2<float32> a = vertices[vertexIndex];
			const vec2<float32> b = vertices[(vertexIndex + 1) % referenceShape.hull->vertexCount];
			const vec2<float32> d = edgeNormalClockwise(a, b);
			const vec2<float32> s = support(incidentShape, -d * RBA);
			const vec2<float32> sT = s * world(incidentBody);

			// The centroid is not involved when calculating the edge distance because it is applied when multiplying by the world matrix.
			const float32 edgeDistance = abs(signedDistanceLPAB(a, b, {0.0f, 0.0f}));
			const vec2<float32> offset = edgeDistance * d * world(referenceBody);

			const float32 separation = signedDistanceLP(d * RA, offset, sT);

			if (separation > output.separation) {
				output.separation = separation;
				output.axis = d;
				output.referenceEdgeIndex = vertexIndex;
				output.referenceBody = &referenceBody;
				output.incidentBody = &incidentBody;
				output.referenceShape = &referenceShape;
				output.incidentShape = &incidentShape;
			}

			if (separation > 0.0f) {
				break;
			}

			vertexIndex += 1;
		}
	}

	// TODO(ci/debugger/undefined-identifier)
	// `output` cannot be debugged because "an unspecified error occurs".
	// See https://developercommunity.visualstudio.com/t/When-a-custom-function-returns-a-custom-/10422600.
	SAT2DOutput SAT2D(const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, const Shape2D& shapeA, const Shape2D& shapeB) {
		SAT2DOutput output{};
		output.separation = min<float32>();

		queryEdgeDirections(rigidBodyA, rigidBodyB, shapeA, shapeB, output);

		if (output.separation > 0.0f) {
			return output;
		}

		queryEdgeDirections(rigidBodyB, rigidBodyA, shapeB, shapeA, output);

		if (output.separation > 0.0f) {
			return output;
		}

		output.axis *= rotationAngle<mat2x2<float32>>(output.referenceBody->rotation);

		return output;
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