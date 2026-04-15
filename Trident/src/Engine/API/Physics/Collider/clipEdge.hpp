// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// Clips an edge against a 2D plane.
	/// Returns a new edge containing the clipped vertices.
	inline ClipFeature2D clipEdge(const ClipFeature2D& edge, Plane2D plane, uint32 edgeIndex) {
		const ClipVertex2D& a = edge.edge[0];
		const ClipVertex2D& b = edge.edge[1];

		const float32 distanceA = signedDistance(a.position, plane);
		const float32 distanceB = signedDistance(b.position, plane);

		ClipFeature2D clippedEdge{};

		if (distanceA <= 0.0f) {
			// Keep vertices that are below the line.
			clippedEdge.edge[clippedEdge.vertexCount] = a;
			clippedEdge.vertexCount += 1;
		}

		if (distanceB <= 0.0f) {
			// Keep vertices that are below the line.
			clippedEdge.edge[clippedEdge.vertexCount] = b;
			clippedEdge.vertexCount += 1;
		}

		if (distanceA * distanceB < 0.0f) {
			// If the vertices are on different sides of the plane, insert the intersection point.
			DEBUG_ASSERT(clippedEdge.vertexCount < 2);

			const float32 t = distanceA / (distanceA - distanceB);

			ClipVertex2D& c = clippedEdge.edge[clippedEdge.vertexCount];
			c.position = lerp(a.position, b.position, t);

			if (distanceA > 0.0f) {
				c.id = a.id;
				c.id.referenceEdgeAIndex = edgeIndex;
				c.id.referenceEdgeBIndex = Contact2DID::NULL_EDGE_INDEX;
			}
			else {
				c.id = b.id;
				c.id.incidentEdgeAIndex = edgeIndex;
				c.id.incidentEdgeBIndex = Contact2DID::NULL_EDGE_INDEX;
			}

			clippedEdge.vertexCount += 1;
		}

		DEBUG_ASSERT(clippedEdge.vertexCount <= 2);

		return clippedEdge;
	}
}