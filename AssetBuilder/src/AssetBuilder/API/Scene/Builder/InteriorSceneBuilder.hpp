// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/StaticSceneBuilder.hpp"

namespace Trident {
	class InteriorSceneBuilder : public StaticSceneBuilder {
		public:
			template<typename FrameType>
			void addFrame(FrameType& frame);

			template<typename RoomType>
			void addRoom(RoomType& room);

		public:
			sequence<float32> floorLevels;
	};
}