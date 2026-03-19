// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// Clips an edge against the 2D plane formed by an offset and a normal.
	/// Returns a new edge containing the clipped vertices.
	inline ClipFeature2D clipEdge(const ClipFeature2D& edge, vec2<float32> normal, float32 offset, uint32 edgeIndex) {
		ClipFeature2D clippedEdge{};

		const float32 d0 = dot(edge.edge[0].position, normal) - offset;
		const float32 d1 = dot(edge.edge[1].position, normal) - offset;

		if (d0 <= 0.0f) {
			// The vertex is below the line. Don't interpolate.
			clippedEdge.edge[clippedEdge.vertexCount] = edge.edge[0];
			clippedEdge.vertexCount += 1;
		}

		if (d1 <= 0.0f) {
			// The vertex is below the line. Don't interpolate.
			clippedEdge.edge[clippedEdge.vertexCount] = edge.edge[1];
			clippedEdge.vertexCount += 1;
		}

		if (d0 * d1 < 0.0f) {
			// If the vertices are on different sides of the plane, interpolate.
			DEBUG_ASSERT(clippedEdge.vertexCount < 2);

			const float32 t = d0 / (d0 - d1);

			ClipVertex2D& clipVertex = clippedEdge.edge[clippedEdge.vertexCount];
			clipVertex.position = lerp(edge.edge[0].position, edge.edge[1].position, t);

			if (d0 > 0.0f) {
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

		// If both vertices are above the line, clip them.
		DEBUG_ASSERT(clippedEdge.vertexCount <= 2);

		return clippedEdge;
	}
}