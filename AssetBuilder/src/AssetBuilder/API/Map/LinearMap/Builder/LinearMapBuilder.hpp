// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"
#include "AssetBuilder/API/Map/LinearMap/RectangularRoomDescriptor.hpp"
#include "AssetBuilder/API/Map/LinearMap/Room.hpp"

namespace Trident {
	class LinearMapBuilder : public MapBuilder {
		public:
			static constexpr uint32 MAX_ROOM_COUNT = 16;

		public:
			explicit LinearMapBuilder(const MapDescriptor& descriptor);

			/// Creates and returns a rectangular room.
			Room& createRectangularRoom(const RectangularRoomDescriptor& descriptor);

			void connectFaces(Face& faceA, Face& faceB, view<vec3<float32>> verticesA, view<vec3<float32>> verticesB);

		private:
			sequence<Room> rooms;
			uint32 roomOffset = 0;
	};
}