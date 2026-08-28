// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Face.hpp"
#include "AssetBuilder/API/Map/FaceVertex.hpp"
#include "AssetBuilder/API/Map/MapDescriptor.hpp"
#include "AssetBuilder/API/Scene/Scene3D.hpp"

namespace Trident {
	class MapBuilder {
		friend Face;

		private:
			static constexpr uint32 MAX_FACE_POSITIONS = 65535;
			static constexpr uint32 MAX_INDICES = 65535;
			static constexpr uint32 MAX_FACES = 256;

		public:
			explicit MapBuilder(const MapDescriptor& descriptor);

			Face& getFace(uint32 index);

			Face& createFace(view<FaceVertex> vertices, svec3<float32> normal);

			/// Extrudes the face `scale` units towards its normal.
			/// This operation moves the current face and creates additional faces on the extruded sides.
			/// 
			/// If `scale` is 0, the operation is ignored.
			void extrudeFace(Face& face, float32 scale);

			BuildScene3D build();

		private:
			MapDescriptor descriptor;

			sequence<svec3<float32>> facePositions;
			sequence<uint32> indices;
			sequence<Face> faces;

			/// Total unique position count.
			uint32 facePositionCount = 0;

			/// Total face vertex count.
			uint32 vertexCount = 0;

			/// Total face triangle count.
			uint32 triangleCount = 0;

			/// Total face count.
			uint32 faceCount = 0;

			uint32 faceIndexCount = 0;
	};
}