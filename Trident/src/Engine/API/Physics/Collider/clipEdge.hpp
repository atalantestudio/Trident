// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Plane.hpp"

namespace Trident {
	/// Clips an edge against a 2D plane.
	/// Returns a new edge containing the clipped vertices.
	inline ClipFeature2D clipEdge(const ClipFeature2D& edge, Plane2D plane, uint32 edgeIndex) {
		ClipFeature2D clippedEdge{};

		// Signed distances between the plane and the edge vertices.
		const float32 distanceA = dot(edge.edge[0].position, plane.normal) - plane.offset;
		const float32 distanceB = dot(edge.edge[1].position, plane.normal) - plane.offset;

		if (distanceA <= 0.0f) {
			// The vertex is below the line: keep it.
			clippedEdge.edge[clippedEdge.vertexCount] = edge.edge[0];
			clippedEdge.vertexCount += 1;
		}

		if (distanceB <= 0.0f) {
			// The vertex is below the line: keep it.
			clippedEdge.edge[clippedEdge.vertexCount] = edge.edge[1];
			clippedEdge.vertexCount += 1;
		}

		if (distanceA * distanceB < 0.0f) {
			// If the vertices are on different sides of the plane, interpolate.
			DEBUG_ASSERT(clippedEdge.vertexCount < 2);

			const float32 t = distanceA / (distanceA - distanceB);

			ClipVertex2D& clipVertex = clippedEdge.edge[clippedEdge.vertexCount];
			clipVertex.position = lerp(edge.edge[0].position, edge.edge[1].position, t);

			if (distanceA > 0.0f) {
				clipVertex.id = edge.edge[0].id;
				clipVertex.id.referenceEdge0Index = edgeIndex;
				clipVertex.id.referenceEdge1Index = Contact2DID::NULL_EDGE_INDEX;
			}
			else {
				clipVertex.id = edge.edge[1].id;
				clipVertex.id.incidentEdge0Index = edgeIndex;
				clipVertex.id.incidentEdge1Index = Contact2DID::NULL_EDGE_INDEX;
			}

			clippedEdge.vertexCount += 1;
		}

		DEBUG_ASSERT(clippedEdge.vertexCount <= 2);

		return clippedEdge;
	}
}