// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Edge2D {
		uint32 indexA;
		uint32 indexB;
	};

	/// See https://en.wikipedia.org/wiki/Doubly_connected_edge_list.
	struct Edge3D {
		/// Index of the origin vertex of this half-edge.
		/// The other vertex is the origin vertex of the next edge.
		uint16 originVertexIndex;

		/// Index of the face this half-edge is linked to.
		/// This is the face at the right of the edge.
		uint16 faceIndex;
	};
}