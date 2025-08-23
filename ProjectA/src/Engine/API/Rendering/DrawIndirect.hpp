#pragma once

namespace ProjectA {
	struct DrawIndirectCommand {
		// The number of vertices to draw.
		uint32 vertexCount;

		// The number of instances to draw.
		uint32 instanceCount;

		// The index of the first vertex to draw.
		uint32 vertexOffset;

		// The index of the first instance to draw.
		uint32 instanceOffset;
	};

	struct DrawIndexedIndirectCommand {
		// The number of vertices to draw.
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