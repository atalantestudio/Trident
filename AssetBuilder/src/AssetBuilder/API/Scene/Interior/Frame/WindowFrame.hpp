// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"
#include "AssetBuilder/API/Scene/Interior/Frame/Frame.hpp"
#include "AssetBuilder/API/Scene/Interior/Room/RectangularRoom.hpp"

namespace Trident {
	struct WindowFrameDescriptor {
		RectangularRoom* room;
		uint32 surfaceIndex;
		float32 floorOffset;
		float32 horizontalOffset;
		svec2<float32> size;
	};

	struct WindowFrame : public Frame {
		explicit WindowFrame(const WindowFrameDescriptor& descriptor) {
			DEBUG_ASSERT(descriptor.room != nullptr);

			SurfaceDescriptor& surfaceDescriptor = descriptor.room->surfaceDescriptors[descriptor.surfaceIndex];

			const Carving carving = {
				.offset = {descriptor.horizontalOffset, surfaceDescriptor.geometry.scale.y - descriptor.size.y - descriptor.floorOffset},
				.size = descriptor.size,
			};

			surfaceDescriptor.carvings.emplace_back(carving);
		}

		const uint32* entranceSurfaceIndex;
		uint32 entranceSubsurfaceIndex;
		const uint32* exitSurfaceIndex;
		uint32 exitSubsurfaceIndex;
	}; 

	template<>
	inline void InteriorSceneBuilder::addFrame(WindowFrame& frame) {}
}