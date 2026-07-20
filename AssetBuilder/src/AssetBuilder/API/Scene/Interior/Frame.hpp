// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Interior/Room.hpp"
#include "AssetBuilder/API/Scene/StaticSurface.hpp"

namespace Trident {
	struct Frame {
		sequence<StaticSurface> surfaces;
	};

	struct DoorFrame : public Frame {
		explicit DoorFrame(Room* firstRoom, Room* secondRoom, uint32 firstSurfaceIndex, uint32 secondSurfaceIndex, svec2<float32> firstOffset, svec2<float32> secondOffset, svec2<float32> size) {
			// TODO(asset-builder/interior/frame): Create surfaces and carvings.
			/* DEBUG_ASSERT(firstRoom != nullptr);
			DEBUG_ASSERT(secondRoom != nullptr);

			StaticSurface& firstSurface = firstRoom->surfaces[firstSurfaceIndex];
			StaticSurface& secondSurface = secondRoom->surfaces[secondSurfaceIndex];

			const Carving firstCarving = {
				.offset = firstOffset,
				.size = size,
			};

			const Carving secondCarving = {
				.offset = secondOffset,
				.size = size,
			};

			firstSurface.carvings.emplace_back(firstCarving);
			secondSurface.carvings.emplace_back(secondCarving); */
		}
	};
}