// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct DrawIndexedIndirectCommand {
		// The number of vertices to draw per instance.
		uint32 indexCount;

		// The number of instances to draw.
		uint32 instanceCount;

		// The index of the first index to draw.
		uint32 indexOffset;

		// A value added to the vertex index before indexing into the vertex buffer.
		sint32 vertexBias;

		// The index of the first instance to draw.
		uint32 instanceOffset;
	};
}