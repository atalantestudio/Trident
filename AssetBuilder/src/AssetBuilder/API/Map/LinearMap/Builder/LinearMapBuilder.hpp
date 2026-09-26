// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"
#include "AssetBuilder/API/Map/LinearMap/RectangularRoomDescriptor.hpp"
#include "AssetBuilder/API/Map/LinearMap/Room.hpp"

namespace Trident {
	class LinearMapBuilder : public MapBuilder {
		public:
			using MapBuilder::MapBuilder;

			/// Creates a rectangular room and returns its index.
			uint32 createRectangularRoom(const RectangularRoomDescriptor& descriptor);

		private:
			std::vector<Room> rooms;
	};
}