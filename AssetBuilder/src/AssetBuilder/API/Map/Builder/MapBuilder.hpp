// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Face.hpp"
#include "AssetBuilder/API/Map/FaceVertex.hpp"
#include "AssetBuilder/API/Map/MapDescriptor.hpp"
#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/API/Scene/Scene3D.hpp"

namespace Trident {
	class MapBuilder : public SceneBuilder {
		friend Face;

		private:
			static constexpr uint32 MAX_FACE_POSITIONS = 65535;
			static constexpr uint32 MAX_INDICES = 65535;
			static constexpr uint32 MAX_FACES = 256;

		public:
			explicit MapBuilder(const MapDescriptor& descriptor);

			Face& getFace(uint32 index);

			/// Creates and returns a new face.
			/// The face must have at least 3 vertices laid out in clockwise order.
			/// Degenerate vertices and edge-crossing vertices are not allowed.
			Face& createFace(view<FaceVertex> vertices, svec3<float32> normal);

			/// Extrudes the face `scale` units towards its normal.
			/// This operation moves the current face and creates additional faces on the extruded sides.
			/// 
			/// If `scale` is 0, the operation is ignored.
			void extrudeFace(Face& face, float32 scale);

			// TODO(asset-builder/map/carve-face): Implement face carving.
			/// Carves a new face inside an existing face.
			/// This operation modifies the vertices of the current face.
			// Face& carve(view<svec3<float32>> vertices);

			BuildScene3D build();

		private:
			void triangulateFaces(BuildScene3D& scene) const;

		protected:
			sequence<svec3<float32>> facePositions;
			sequence<uint32> indices;
			sequence<Face> faces;

			/// Total unique position count.
			uint32 facePositionCount = 0;

			uint32 faceIndexCount = 0;

			/// Total face vertex count.
			uint32 vertexCount = 0;

			/// Total face triangle count.
			uint32 triangleCount = 0;

			/// Total face count.
			uint32 faceCount = 0;

		private:
			MapDescriptor descriptor;
	};
}