// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class MapBuilder;

	class Face {
		friend MapBuilder;

		public:
			/// Returns a face vertex.
			/// `indexOffset` is used to read into the index buffer.
			svec3<float32>& getVertex(uint32 indexOffset);

			/// Carves and returns a new face.
			/// This operation modifies the current face.
			Face carve(view<svec3<float32>> vertices);

			/// Extrudes the face `scale` units towards its normal.
			/// This operation moves the current face and creates additional faces on the extruded sides.
			/// 
			/// If `scale` is 0, the operation is ignored.
			Face extrude(float32 scale);

		private:
			explicit Face(MapBuilder* builder);

		public:
			/// One texture coordinate per vertex.
			view<svec2<float32>> textureCoordinates;

			svec3<float32> normal;

			std::vector<uint32> indices;

		private:
			MapBuilder* builder;
	};
}