// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// The line the edge is clipped against is formed by the `normal` and `offset` parameters.
	inline ClipFeature2D clipEdge(const ClipFeature2D& edge, vec2<float32> normal, float32 offset, uint32 edgeIndex) {
		const float32 d0 = dot(edge.edge[0].position, normal) - offset;
		const float32 d1 = dot(edge.edge[1].position, normal) - offset;

		ClipFeature2D clippedEdge{};
		uint32 clippedEdgeVertexCount = 0;

		if (d0 <= 0.0f) {
			// The vertex is below the line. Don't interpolate.
			clippedEdge.edge[clippedEdgeVertexCount] = edge.edge[0];
			clippedEdgeVertexCount += 1;
		}

		if (d1 <= 0.0f) {
			// The vertex is below the line. Don't interpolate.
			clippedEdge.edge[clippedEdgeVertexCount] = edge.edge[1];
			clippedEdgeVertexCount += 1;
		}

		// If the vertices are on different sides of the plane, interpolate.
		if (d0 * d1 < 0.0f) {
			DEBUG_ASSERT(clippedEdgeVertexCount < 2);

			const float32 t = d0 / (d0 - d1);

			clippedEdge.edge[clippedEdgeVertexCount].position = lerp(edge.edge[0].position, edge.edge[1].position, t);

			if (d0 > 0.0f) {
				clippedEdge.edge[clippedEdgeVertexCount].id = edge.edge[0].id;
				clippedEdge.edge[clippedEdgeVertexCount].id.referenceEdge0Index = edgeIndex;
				clippedEdge.edge[clippedEdgeVertexCount].id.referenceEdge1Index = Contact2DID::NULL_EDGE_INDEX;
			}
			else {
				clippedEdge.edge[clippedEdgeVertexCount].id = edge.edge[1].id;
				clippedEdge.edge[clippedEdgeVertexCount].id.incidentEdge0Index = edgeIndex;
				clippedEdge.edge[clippedEdgeVertexCount].id.incidentEdge1Index = Contact2DID::NULL_EDGE_INDEX;
			}

			clippedEdgeVertexCount += 1;
		}

		// If both vertices are above the line, clip them.

		DEBUG_ASSERT(clippedEdgeVertexCount <= 2);

		clippedEdge.vertexCount = clippedEdgeVertexCount;

		return clippedEdge;
	}
}