// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"
#include "AssetBuilder/API/Scene/Interior/Frame/Frame.hpp"
#include "AssetBuilder/API/Scene/Interior/Room/Room.hpp"

namespace Trident {
	struct DoorFrameDescriptor {
		Room* entranceRoom;
		uint32 entranceSurfaceIndex;
		float32 entranceFloorOffset;
		float32 entranceHorizontalOffset;
		Room* exitRoom;
		uint32 exitSurfaceIndex;
		float32 exitFloorOffset;
		float32 exitHorizontalOffset;
		svec2<float32> size;
	};

	// TODO(asset-builder/scene/interior/frame): Add trimmed variant with custom material index.
	struct DoorFrame : public Frame {
		explicit DoorFrame(const DoorFrameDescriptor& descriptor) {
			DEBUG_ASSERT(descriptor.entranceRoom != nullptr);
			DEBUG_ASSERT(descriptor.exitRoom != nullptr);

			SurfaceDescriptor& entranceDescriptor = descriptor.entranceRoom->surfaceDescriptors[descriptor.entranceSurfaceIndex];
			SurfaceDescriptor& exitDescriptor = descriptor.exitRoom->surfaceDescriptors[descriptor.exitSurfaceIndex];

			const Carving entranceCarving = {
				.offset = {descriptor.entranceHorizontalOffset, entranceDescriptor.geometry.scale.y - descriptor.size.y - descriptor.entranceFloorOffset},
				.size = descriptor.size,
			};
			const Carving exitCarving = {
				.offset = {descriptor.exitHorizontalOffset, exitDescriptor.geometry.scale.y - descriptor.size.y - descriptor.exitFloorOffset},
				.size = descriptor.size,
			};

			entranceDescriptor.carvings.emplace_back(entranceCarving);
			exitDescriptor.carvings.emplace_back(exitCarving);
		}
	};

	template<>
	inline void InteriorSceneBuilder::addFrame(DoorFrame& frame) {
		// TODO(asset-builder/scene/interior/frame): Add support for multiple edge snapping.
	}
}