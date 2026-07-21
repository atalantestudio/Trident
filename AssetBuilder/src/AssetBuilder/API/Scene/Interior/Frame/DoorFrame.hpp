// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"
#include "AssetBuilder/API/Scene/Interior/Frame/Frame.hpp"
#include "AssetBuilder/API/Scene/Interior/Room/RectangularRoom.hpp"

namespace Trident {
	struct DoorFrameDescriptor {
		RectangularRoom* entranceRoom;
		uint32 entranceSurfaceIndex;
		float32 entranceFloorOffset;
		float32 entranceHorizontalOffset;
		RectangularRoom* exitRoom;
		uint32 exitSurfaceIndex;
		float32 exitFloorOffset;
		float32 exitHorizontalOffset;
		svec2<float32> size;
	};

	// TODO(asset-builder/scene/interior/frame): Add trimmed variant with custom material index.
	struct DoorFrame : public Frame {
		static constexpr uint32 CEILING = 0;
		static constexpr uint32 RIGHT_WALL = 1;
		static constexpr uint32 FLOOR = 2;
		static constexpr uint32 LEFT_WALL = 3;

		explicit DoorFrame(const DoorFrameDescriptor& descriptor) {
			DEBUG_ASSERT(descriptor.entranceRoom != nullptr);
			DEBUG_ASSERT(descriptor.exitRoom != nullptr);

			const SurfaceDescriptor& floorDescriptor = descriptor.entranceRoom->surfaceDescriptors[RectangularRoom::FLOOR];
			const SurfaceDescriptor& wallDescriptor = descriptor.entranceRoom->surfaceDescriptors[RectangularRoom::FRONT_WALL];
			const SurfaceDescriptor& ceilingDescriptor = descriptor.entranceRoom->surfaceDescriptors[RectangularRoom::CEILING];

			surfaceDescriptors = {
				{
					.textureFiltering = ceilingDescriptor.textureFiltering,
					.materialIndex = ceilingDescriptor.materialIndex,
					.luxelScale = ceilingDescriptor.luxelScale,
				},
				{
					.textureFiltering = wallDescriptor.textureFiltering,
					.materialIndex = wallDescriptor.materialIndex,
					.luxelScale = wallDescriptor.luxelScale,
				},
				{
					.textureFiltering = floorDescriptor.textureFiltering,
					.materialIndex = floorDescriptor.materialIndex,
					.luxelScale = floorDescriptor.luxelScale,
				},
				{
					.textureFiltering = wallDescriptor.textureFiltering,
					.materialIndex = wallDescriptor.materialIndex,
					.luxelScale = wallDescriptor.luxelScale,
				},
			};

			entranceSurfaceIndex = &descriptor.entranceRoom->surfaceIndices[descriptor.entranceSurfaceIndex];
			exitSurfaceIndex = &descriptor.exitRoom->surfaceIndices[descriptor.exitSurfaceIndex];

			SurfaceDescriptor& entranceDescriptor = descriptor.entranceRoom->surfaceDescriptors[descriptor.entranceSurfaceIndex];
			SurfaceDescriptor& exitDescriptor = descriptor.exitRoom->surfaceDescriptors[descriptor.exitSurfaceIndex];

			entranceSubsurfaceIndex = static_cast<uint32>(entranceDescriptor.carvings.size());
			exitSubsurfaceIndex = static_cast<uint32>(exitDescriptor.carvings.size());

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

		sequence<DualSnapSurfaceDescriptor> surfaceDescriptors;
		const uint32* entranceSurfaceIndex;
		uint32 entranceSubsurfaceIndex;
		const uint32* exitSurfaceIndex;
		uint32 exitSubsurfaceIndex;
	}; 

	template<>
	inline void InteriorSceneBuilder::addFrame(DoorFrame& frame) {
		const StaticSurface* entranceSurface = &surfaces[*frame.entranceSurfaceIndex];
		const StaticSurface* exitSurface = &surfaces[*frame.exitSurfaceIndex];

		const StaticSubsurface& entranceSubsurface = entranceSurface->hollowSubsurfaces[frame.entranceSubsurfaceIndex];
		const StaticSubsurface& exitSubsurface = exitSurface->hollowSubsurfaces[frame.exitSubsurfaceIndex];

		snapSurface(frame.surfaceDescriptors[frame.CEILING], entranceSubsurface.getEdge(0), exitSubsurface.getEdge(0));
		snapSurface(frame.surfaceDescriptors[frame.RIGHT_WALL], entranceSubsurface.getEdge(1), exitSubsurface.getEdge(3));
		snapSurface(frame.surfaceDescriptors[frame.FLOOR], entranceSubsurface.getEdge(2), exitSubsurface.getEdge(2));
		snapSurface(frame.surfaceDescriptors[frame.LEFT_WALL], entranceSubsurface.getEdge(3), exitSubsurface.getEdge(1));
	}
}