#pragma once

#include "Engine/API/Physics/Contact/Contact.hpp"
#include "Engine/API/Physics/Feature2D.hpp"

namespace ProjectA {
	class ContactManifold2D {
		public:
			void setContactCount(uint32 contactCount) {
				this->contactCount = contactCount - 1;
			}

			uint32 getContactCount() const {
				return contactCount + 1;
			}

		public:
			vec2<float32> normal;
			vec2<float32> tangent;
			Contact2D contacts[2];

		private:
			// TODO: Use fewer bits.
			uint8 contactCount : 1;
	};

	// Finds and returns the incident edge feature on a convex hull.
	static Feature2D findIncidentEdge(const PhysicalObjectCopy2D& incidentObject, const vec2<float32>& referenceEdgeNormal) {
		const mat2x2<float32> R = mat2x2_rotation(-incidentObject.rotation);

		float32 minDotProduct = max<float32>();
		Feature2D incidentEdge;

		// TODO: Expose edges/normals from ConvexHullGeometry.
		for (uint32 edgeVertexOffset = 0; edgeVertexOffset < incidentObject.geometry->getVertexCount(); edgeVertexOffset += 1) {
			const vec2<float32>& a = incidentObject.geometry->vertices[(edgeVertexOffset + 0) % incidentObject.geometry->getVertexCount()];
			const vec2<float32>& b = incidentObject.geometry->vertices[(edgeVertexOffset + 1) % incidentObject.geometry->getVertexCount()];

			const vec2<float32> edgeNormal = edgeNormalClockwise(a, b) * R;

			const float32 dotProduct = dot(referenceEdgeNormal, edgeNormal);

			if (dotProduct < minDotProduct) {
				minDotProduct = dotProduct;
				incidentEdge.edge[0] = a;
				incidentEdge.edge[1] = b;
			}
		}

		incidentEdge.edge[0] = multiplyTransform(incidentEdge.edge[0], incidentObject.transform);
		incidentEdge.edge[1] = multiplyTransform(incidentEdge.edge[1], incidentObject.transform);

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

			// clippedEdge.edge[clippedEdgeVertexCount] = lerp(edge.edge[0], edge.edge[1], t);
			clippedEdge.edge[clippedEdgeVertexCount] = edge.edge[0] + t * (edge.edge[1] - edge.edge[0]);
			clippedEdgeVertexCount += 1;
		}

		// If both vertices are above the line, clip them.

		DEBUG_ASSERT(clippedEdgeVertexCount <= 2);

		clippedEdge.setVertexCount(clippedEdgeVertexCount);

		return clippedEdge;
	}
}