// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/RectangularRoomDescriptor.hpp"
#include "AssetBuilder/API/Map/Room.hpp"
#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	class LinearMapBuilder : public SceneBuilder {
		public:
			using SceneBuilder::SceneBuilder;

			/// Creates a rectangular room and returns its index.
			uint32 createRectangularRoom(const RectangularRoomDescriptor& descriptor);

		private:
			std::vector<Room> rooms;
	};
}