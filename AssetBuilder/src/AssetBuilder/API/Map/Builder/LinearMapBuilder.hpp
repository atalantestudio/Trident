// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"
#include "AssetBuilder/API/Map/RectangularRoomDescriptor.hpp"
#include "AssetBuilder/API/Map/Room.hpp"

namespace Trident {
	class LinearMapBuilder : public MapBuilder {
		public:
			explicit LinearMapBuilder(const MapDescriptor& descriptor);

			/// Creates a rectangular room and returns its index.
			uint32 createRectangularRoom(const RectangularRoomDescriptor& descriptor);

		private:
			std::vector<Room> rooms;
	};
}