// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/SAT/SATCircleConvexHull.hpp"

namespace Trident {
	static void queryEdgeDirections(const RigidBody2D& referenceBody, const RigidBody2D& incidentBody, const Shape2D& referenceShape, const Shape2D& incidentShape, SATCircleConvexHullOutput2D& output) {
		const vec2<float32>* referenceVertices = referenceShape.hull->getVertices();

		const mat3x3<float32> referenceWorld = referenceBody.getWorld();
		const mat3x3<float32> incidentWorld = incidentBody.getWorld();
		const Rotation2D RBA = referenceBody.transform.rotation * inverse(incidentBody.transform.rotation);

		uint16 indexA = 0;

		while (indexA < referenceShape.hull->vertexCount) {
			const uint16 indexB = (indexA + 1) % referenceShape.hull->vertexCount;
			const vec2<float32> a = referenceVertices[indexA];
			const vec2<float32> b = referenceVertices[indexB];
			const vec2<float32> d = edgeNormalClockwise(a, b);
			const vec2<float32> s = vertexSupport(incidentShape, -d * RBA) * incidentWorld;

			const float32 separation = signedDistanceLP(d * referenceBody.transform.rotation, d * dot(a, d) * referenceWorld, s);

			if (separation > output.separation) {
				output.separation = separation;
				output.axis = d;
				output.incidentSupportPoint = s;
			}

			if (separation > 0.0f) {
				break;
			}

			indexA += 1;
		}
	}

	// TODO(ci/debugger/undefined-identifier)
	// The returned object cannot be debugged because "an unspecified error occurs".
	// See https://developercommunity.visualstudio.com/t/When-a-custom-function-returns-a-custom-/10422600.
	SATCircleConvexHullOutput2D SATCircleConvexHull2D(const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, const Shape2D& shapeA, const Shape2D& shapeB) {
		DEBUG_ASSERT(shapeA.type == ShapeType2D::CIRCLE);
		DEBUG_ASSERT(shapeB.type == ShapeType2D::CONVEX_HULL);

		SATCircleConvexHullOutput2D output{};
		output.separation = min<float32>();

		queryEdgeDirections(rigidBodyB, rigidBodyA, shapeB, shapeA, output);

		if (output.separation > 0.0f) {
			return output;
		}

		// TODO(physics/2d/sat-circle-convex-hull): Since the incident body is always the circle, does the axis need to be corrected?
		output.axis *= rigidBodyB.transform.rotation;

		return output;
	}
}