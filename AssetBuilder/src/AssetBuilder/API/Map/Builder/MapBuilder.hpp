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
			static constexpr uint32 MAX_FACE_TEXTURE_COORDINATES = 65535;

		public:
			explicit MapBuilder(const MapDescriptor& descriptor);

			Face& createFace(view<FaceVertex> vertices, svec3<float32> normal);

			BuildScene3D build();

		private:
			MapDescriptor descriptor;

			std::vector<Face> faces;
			sequence<svec3<float32>> facePositions;
			sequence<svec2<float32>> faceTextureCoordinates;

			uint32 lightmapCount = 0;
			uint32 facePositionCount = 0;
			uint32 faceVertexCount = 0;
			uint32 faceTextureCoordinateCount = 0;

			/// Total face triangle count.
			uint32 triangleCount = 0;

			uint32 faceIndexCount = 0;
	};
}