// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class Face {
		public:
			/// Carves and returns a new face.
			/// This operation modifies the current face.
			Face carve(view<svec3<float32>> vertices);

		public:
			uint32 vertexCount;
			range<uint32, uint32> indexRange;
			svec3<float32> normal;
	};
}