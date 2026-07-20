// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

namespace Trident {
	class InteriorSceneBuilder : public StaticSceneBuilder {
		public:
			explicit InteriorSceneBuilder(std::filesystem::path rootOutputPath);

			template<typename FrameType>
			void addFrame(FrameType& frame);

			template<typename RoomType>
			void addRoom(RoomType& room);

		public:
			sequence<float32> floorLevels;
	};
}