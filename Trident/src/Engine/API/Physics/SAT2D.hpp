// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/ConvexHullGeometry2D.hpp"
#include "Engine/API/Physics/Feature2D.hpp"
#include "Engine/API/Physics/RigidBody2D.hpp"

namespace Trident {
	struct SAT2DOutput {
		// A separation less than or equal to 0 indicates overlap between the shapes.
		float32 separation;

		// Normal of the reference edge.
		vec2<float32> normal;

		// Index of the reference edge in the geometry vertex array.
		uint32 referenceEdgeIndex;

		// Closest edge on the reference body to the incident body.
		// The vertices are already transformed.
		Feature2D referenceEdge;

		// Support point on the incident body, in the direction opposite to the reference edge's normal.
		// This is the closest point to the reference edge.
		vec2<float32> incidentSupportPoint;

		const RigidBody2D* referenceBody;
		const RigidBody2D* incidentBody;

		const Geometry2D* referenceGeometry;
		const Geometry2D* incidentGeometry;
	};

	static void queryEdgeDirections(const RigidBody2D& referenceBody, const RigidBody2D& incidentBody, const Geometry2D& referenceGeometry, const Geometry2D& incidentGeometry, SAT2DOutput& output) {
		const mat2x2<float32> R = mat2x2<float32>::rotation(incidentBody.rotation - referenceBody.rotation);

		uint32 edgeIndex = 0;

		// TODO: Expose edge normals from ConvexHullGeometry?
		while (edgeIndex < referenceGeometry.getEdges().count()) {
			const Edge edge = referenceGeometry.getEdges()[edgeIndex];
			const vec2<float32> a = referenceGeometry.getVertices()[edge.indexA];
			const vec2<float32> b = referenceGeometry.getVertices()[edge.indexB];
			const vec2<float32> d = edgeNormalClockwise(a, b);
			const vec2<float32> s = incidentGeometry.support(-d * R);
			const vec2<float32> aT = multiplyTransform(a, referenceBody.transform);
			const vec2<float32> bT = multiplyTransform(b, referenceBody.transform);
			const vec2<float32> sT = multiplyTransform(s, incidentBody.transform);
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
				output.referenceGeometry = &referenceGeometry;
				output.incidentGeometry = &incidentGeometry;
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
	static SAT2DOutput SAT2D(const RigidBody2D& A, const RigidBody2D& B, const Geometry2D& geometryA, const Geometry2D& geometryB) {
		SAT2DOutput output{};
		output.separation = min<float32>();
		output.referenceEdge.vertexCount = 2;

		queryEdgeDirections(A, B, geometryA, geometryB, output);

		if (output.separation > 0.0f) {
			return output;
		}

		queryEdgeDirections(B, A, geometryB, geometryA, output);

		if (output.separation > 0.0f) {
			return output;
		}

		DEBUG_ASSERT(output.referenceBody != nullptr);

		output.normal *= mat2x2<float32>::rotation(-output.referenceBody->rotation);

		return output;
	}
}