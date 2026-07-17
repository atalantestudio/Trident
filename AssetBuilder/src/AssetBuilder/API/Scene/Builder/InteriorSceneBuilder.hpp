// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"
#include "AssetBuilder/API/Scene/Interior/Room.hpp"

namespace Trident {
	class InteriorSceneBuilder : public StaticSceneBuilder {
		public:
			explicit InteriorSceneBuilder(std::filesystem::path rootOutputPath, view<char8> assetName);

			void addRoom(const Room& room);

		public:
			sequence<float32> floorLevels;
	};
}