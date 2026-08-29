// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Face {
		uint32 vertexCount;

		// The range's count is not used (equal to the vertex count).
		range<uint32, uint32> indexRange;

		svec3<float32> normal;
	};
}