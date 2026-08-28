// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class MapBuilder;

	class Face {
		friend MapBuilder;

		public:
			explicit Face() = default;

			/// Returns a face position.
			/// `index` is used to read into the index buffer.
			svec3<float32>& getPosition(uint32 index);

			/// Carves and returns a new face.
			/// This operation modifies the current face.
			Face carve(view<svec3<float32>> vertices);

		private:
			explicit Face(MapBuilder* builder);

		public:
			uint32 vertexCount;
			range<uint32, uint32> indexRange;
			svec3<float32> normal;

		private:
			MapBuilder* builder;
	};
}