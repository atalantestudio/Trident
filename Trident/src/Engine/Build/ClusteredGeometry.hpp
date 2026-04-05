// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct ClusteredGeometry {
		// Number of vertex positions.
		uint32 vertexCount;

		// Per instance cluster count.
		uint32 clusterCount;

		// Index of the first cluster belonging to this geometry.
		uint32 clusterOffset;

		// Number of instances using this geometry.
		uint32 instanceCount;

		// Offset to be added to the index before indexing into the vertex position and normal buffers.
		uint32 vertexOffset;

		// Offset to be added to the index before indexing into the vertex texture coordinate buffer.
		uint32 vertexTextureCoordinateOffset;
	};
}