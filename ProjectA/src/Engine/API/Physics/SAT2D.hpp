#pragma once

#include "Engine/API/Geometry/ConvexHullGeometry2D.hpp"
#include "Engine/API/Physics/RigidBody2D.hpp"

namespace ProjectA {
	struct SAT2DOutput {
		// A separation less than or equal to 0 indicates overlap between the shapes.
		float32 separation;

		// Normal of the reference edge.
		vec2<float32> normal;

		// Closest edge on the reference body to the incident body.
		Feature2D referenceEdge;

		// Index of the reference edge in the geometry vertex array.
		uint32 referenceEdgeIndex;

		// Support point on the incident body, in the direction opposite to the reference edge's normal.
		// This is the closest point to the reference edge.
		vec2<float32> incidentSupportPoint;

		// Pointer to the reference body.
		const RigidBody2D* referenceBody;

		// Pointer to the incident body.
		const RigidBody2D* incidentBody;
	};

	static void queryEdgeDirections(const RigidBody2D& A, const RigidBody2D& B, SAT2DOutput& output) {
		const ref<ConvexHullGeometry2D>& geometry = std::static_pointer_cast<ConvexHullGeometry2D>(A.geometry);
		const mat2x2<float32> R = mat2x2<float32>::rotation(B.rotation - A.rotation);

		// TODO: Expose edge normals from ConvexHullGeometry.
		for (uint32 edgeIndex = 0; edgeIndex < geometry->edges.count(); edgeIndex += 1) {
			const Edge& edge = geometry->edges[edgeIndex];
			const vec2<float32>& a = geometry->vertices[edge.indexA];
			const vec2<float32>& b = geometry->vertices[edge.indexB];

			const vec2<float32> d = edgeNormalClockwise(a, b) * R;

			const vec2<float32> s = B.geometry->support(-d);
			const vec2<float32> sT = multiplyTransform(s, B.transform);

			const vec2<float32> aT = multiplyTransform(a, A.transform);
			const vec2<float32> bT = multiplyTransform(b, A.transform);

			const float32 separation = signedDistanceLinePoint(aT, bT, sT);

			// NOTE: Use >= to account restore B as the reference body if possible.
			if (separation >= output.separation) {
				output.separation = separation;
				output.normal = d;
				output.referenceEdge.edge[0] = aT;
				output.referenceEdge.edge[1] = bT;
				output.referenceEdgeIndex = edgeIndex;
				output.incidentSupportPoint = sT;
				output.referenceBody = &A;
				output.incidentBody = &B;
			}

			if (separation > 0.0f) {
				break;
			}
		}
	}

	// Separating Axis Theorem algorithm implementation in 2D.
	// TODO: Basic optimizations:
	//       - Don't test the same separating axis multiple times
	//       - Don't test parallel axes (https://dyn4j.org/2010/01/sat)
	//       - Reuse the separation axis from the last run to make the algorithm O(1) in non-intersection cases. But what if the separation axis changes e.g. due to rotation?
	static SAT2DOutput SAT2D(const RigidBody2D& A, const RigidBody2D& B) {
		SAT2DOutput output;
		output.separation = min<float32>();
		output.referenceEdge.setVertexCount(2);

		queryEdgeDirections(A, B, output);

		if (output.separation > 0.0f) {
			return output;
		}

		queryEdgeDirections(B, A, output);

		if (output.separation > 0.0f) {
			return output;
		}

		// Correct normal direction.
		output.normal = output.normal * mat2x2<float32>::rotation(-output.incidentBody->rotation);

		return output;
	}
}