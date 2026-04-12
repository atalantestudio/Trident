// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Edge {
		uint32 indexA;
		uint32 indexB;
	};

	/// See https://en.wikipedia.org/wiki/Doubly_connected_edge_list.
	/// 
	/// NOTE: Edges and twin edges must be stored one after the other.
	/// This allows to efficiently iterate over unique edges by ignoring even or odd entries.
	struct HalfEdge {
		/// Index of the origin vertex of this half-edge.
		/// The other vertex is the origin vertex of the next edge.
		uint16 originVertexIndex;

		/// Index of the next half-edge.
		uint16 nextEdgeIndex;

		/// Index of the face this half-edge is linked to.
		uint16 faceIndex;
	};
}