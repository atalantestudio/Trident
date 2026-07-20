// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"
#include "AssetBuilder/API/Scene/Interior/Room/Room.hpp"

namespace Trident {
	struct RectangularRoom : public Room {
		svec2<float32> position;
		svec2<float32> size;
		float32 floorLevel;
		float32 ceilingLevel;
		svec2<float32> luxelScale;
		uint32 floorMaterialIndex;
		uint32 wallMaterialIndex;
		uint32 ceilingMaterialIndex;
	};

	template<>
	inline void InteriorSceneBuilder::addRoom(const RectangularRoom& room) {
		const float32 sizeX = room.size.x;
		const float32 sizeY = room.ceilingLevel - room.floorLevel;
		const float32 sizeZ = room.size.y;
		const float32 halfSizeY = sizeY * 0.5f;

		const StaticGeometry3D floorGeometry = InteriorSceneBuilder::createWallGeometry({sizeX, sizeZ});
		const StaticGeometry3D frontWallGeometry = InteriorSceneBuilder::createWallGeometry({sizeX, sizeY});
		const StaticGeometry3D rightWallGeometry = InteriorSceneBuilder::createWallGeometry({sizeZ, sizeY});

		const SurfaceDescriptor floorDescriptor = {
			.geometry = floorGeometry,
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {1.0f / 128.0f, 1.0f / 128.0f},
			},
			.materialIndex = room.floorMaterialIndex,
			.luxelScale = room.luxelScale,
			.carvings = {},
		};

		const SurfaceDescriptor ceilingDescriptor = {
			.geometry = floorGeometry,
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {1.0f / 128.0f, 1.0f / 128.0f},
			},
			.materialIndex = room.ceilingMaterialIndex,
			.luxelScale = room.luxelScale,
			.carvings = {},
		};

		const SurfaceDescriptor frontWallDescriptor = {
			.geometry = frontWallGeometry,
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {1.0f / 128.0f, 1.0f / 128.0f},
			},
			.materialIndex = room.wallMaterialIndex,
			.luxelScale = room.luxelScale,
			.carvings = {},
		};

		const SurfaceDescriptor rightWallDescriptor = {
			.geometry = rightWallGeometry,
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {1.0f / 128.0f, 1.0f / 128.0f},
			},
			.materialIndex = room.wallMaterialIndex,
			.luxelScale = room.luxelScale,
			.carvings = {},
		};

		const uint32 frontWallIndex = placeSurface(
			frontWallDescriptor,
			{
				.translation = {room.position.x, room.floorLevel + halfSizeY, room.position.y},
				.axis = {0.0f, 1.0f, 0.0f},
				.angle = 0.0f,
			}
		);

		const uint32 rightWallIndex = snapSurface(rightWallDescriptor, frontWallIndex, 1, 3, radians(90.0_deg));

		snapSurface(rightWallDescriptor, frontWallIndex, 3, 1, radians(90.0_deg));
		snapSurface(frontWallDescriptor, rightWallIndex, 1, 3, radians(180.0_deg));
		snapSurface(floorDescriptor, frontWallIndex, 2, 0, radians(90.0_deg));
		snapSurface(ceilingDescriptor, frontWallIndex, 0, 2, radians(90.0_deg));
	}
}