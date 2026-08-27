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

		public:
			explicit MapBuilder(const MapDescriptor& descriptor);

			Face createFace(view<FaceVertex> vertices, svec3<float32> normal);

			BuildScene3D build();

		private:
			MapDescriptor descriptor;

			std::vector<svec3<float32>> facePositions;

			uint32 lightmapCount = 0;
	};
}