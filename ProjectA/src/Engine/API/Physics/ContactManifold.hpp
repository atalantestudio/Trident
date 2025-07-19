#pragma once

#include "Engine/API/Physics/Feature2D.hpp"

namespace ProjectA {
	struct Contact2D {
		vec2<float32> position;

		// Distance to the normal of the reference edge.
		float32 separation;

		// A's position to the contact.
		vec2<float32> rA;

		// B's position to the contact.
		vec2<float32> rB;

		float32 massNormal;
		float32 massTangent;
		float32 bias;

		float32 accumulatedNormalImpulse;
		float32 accumulatedTangentImpulse;
	};

	struct ContactManifold2D {
		vec2<float32> contactNormal;

		// TODO: Use fewer bits.
		uint8 contactCount;

		Contact2D contacts[2];
	};

	// Finds and returns the incident edge feature on a convex hull.
	static Feature2D findIncidentEdge(const PhysicalObjectCopy2D& incidentObject, const vec2<float32>& referenceEdgeNormal) {
		const ref<Geometry2D>& geometry = incidentObject.geometry;

		float32 minDotProduct = max<float32>();
		vec2<float32> a;
		vec2<float32> b;

		// TODO: Expose edges/normals from ConvexHullGeometry.
		for (uint32 edgeIndex = 0, edgeVertexOffset = 0; edgeVertexOffset < incidentObject.geometry->getVertexCount(); edgeIndex += 1, edgeVertexOffset += 1) {
			const vec2<float32>& edge1 = incidentObject.geometry->vertices[(edgeVertexOffset + 0) % incidentObject.geometry->getVertexCount()];
			const vec2<float32>& edge2 = incidentObject.geometry->vertices[(edgeVertexOffset + 1) % incidentObject.geometry->getVertexCount()];
			const vec2<float32> transformedEdge1 = multiplyTransform(edge1, incidentObject.transform);
			const vec2<float32> transformedEdge2 = multiplyTransform(edge2, incidentObject.transform);
			const vec2<float32> edgeNormal = edgeNormalClockwise(transformedEdge1, transformedEdge2);

			const float32 dotProduct = dot(edgeNormal, referenceEdgeNormal);

			if (dotProduct < minDotProduct) {
				minDotProduct = dotProduct;
				a = transformedEdge1;
				b = transformedEdge2;
			}
		}

		Feature2D incidentEdge;
		incidentEdge.edge[0] = a;
		incidentEdge.edge[1] = b;

		return incidentEdge;
	}

	// `normal` and `offset` form the line equation.
	static Feature2D clipEdge(const Feature2D& edge, const vec2<float32>& normal, float32 offset) {
		Feature2D clippedEdge;
		uint32 clippedEdgeVertexCount = 0;

		const float32 d0 = dot(edge.edge[0], normal) - offset;

		if (d0 <= 0.0f) {
			// This vertex is behind the line; don't interpolate.
			clippedEdge.edge[clippedEdgeVertexCount] = edge.edge[0];
			clippedEdgeVertexCount += 1;
		}

		const float32 d1 = dot(edge.edge[1], normal) - offset;

		if (d1 <= 0.0f) {
			// This vertex is behind the line; don't interpolate.
			clippedEdge.edge[clippedEdgeVertexCount] = edge.edge[1];
			clippedEdgeVertexCount += 1;
		}

		// If one vertex is above and the other is below, interpolate.
		if (d0 * d1 < 0.0f) {
			DEBUG_ASSERT(clippedEdgeVertexCount < 2);

			const float32 t = d0 / (d0 - d1);

			clippedEdge.edge[clippedEdgeVertexCount] = lerp(edge.edge[0], edge.edge[1], t);
			clippedEdgeVertexCount += 1;
		}

		// If both vertices are above the line, clip them.

		DEBUG_ASSERT(clippedEdgeVertexCount <= 2);

		clippedEdge.setVertexCount(clippedEdgeVertexCount);

		return clippedEdge;
	}
}