// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

namespace Trident {
	class InteriorSceneBuilder : public StaticSceneBuilder {
		public:
			explicit InteriorSceneBuilder(std::filesystem::path rootOutputPath, view<char8> assetName);

			uint32 addFloorLevel(float32 level);

			void addRoom(uint32 baseFloorLevelIndex, float32 sizeX, float32 sizeZ);

		private:
			std::vector<float32> floorLevels;
	};
}