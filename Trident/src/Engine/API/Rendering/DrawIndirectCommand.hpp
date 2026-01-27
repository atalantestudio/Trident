// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct DrawIndirectCommand {
		// The number of vertices to draw per instance.
		uint32 vertexCount;

		// The number of instances to draw.
		uint32 instanceCount;

		// The index of the first vertex to draw.
		uint32 vertexOffset;

		// The index of the first instance to draw.
		uint32 instanceOffset;
	};
}