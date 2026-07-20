// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"
#include "AssetBuilder/API/Scene/Interior/Room/Room.hpp"

namespace Trident {
	struct RectangularRoomDescriptor {
		svec2<float32> position;
		svec2<float32> size;
		float32 floorLevel;
		float32 ceilingLevel;
		svec2<float32> luxelScale;
		uint32 floorMaterialIndex;
		uint32 wallMaterialIndex;
		uint32 ceilingMaterialIndex;
	};

	struct RectangularRoom : public Room {
		explicit RectangularRoom(const RectangularRoomDescriptor& descriptor) {
			const float32 sizeX = descriptor.size.x;
			const float32 sizeY = descriptor.ceilingLevel - descriptor.floorLevel;
			const float32 sizeZ = descriptor.size.y;

			frontWallTranslation = {descriptor.position.x, descriptor.floorLevel + sizeY * 0.5f, descriptor.position.y};

			const StaticGeometry3D floorGeometry = InteriorSceneBuilder::createWallGeometry({sizeX, sizeZ});
			const StaticGeometry3D frontWallGeometry = InteriorSceneBuilder::createWallGeometry({sizeX, sizeY});
			const StaticGeometry3D rightWallGeometry = InteriorSceneBuilder::createWallGeometry({sizeZ, sizeY});

			// TODO(asset-builder/scene/interior/room): Expose properties per individual room surface.
			const TextureFilteringDescriptor textureFiltering = {
				.scale = {1.0f / 128.0f, 1.0f / 128.0f},
			};

			const SurfaceDescriptor frontWallDescriptor = {
				.geometry = frontWallGeometry,
				.textureFiltering = textureFiltering,
				.materialIndex = descriptor.wallMaterialIndex,
				.luxelScale = descriptor.luxelScale,
			};
			const SurfaceDescriptor rightWallDescriptor = {
				.geometry = rightWallGeometry,
				.textureFiltering = textureFiltering,
				.materialIndex = descriptor.wallMaterialIndex,
				.luxelScale = descriptor.luxelScale,
			};
			const SurfaceDescriptor floorDescriptor = {
				.geometry = floorGeometry,
				.textureFiltering = textureFiltering,
				.materialIndex = descriptor.floorMaterialIndex,
				.luxelScale = descriptor.luxelScale,
			};
			const SurfaceDescriptor ceilingDescriptor = {
				.geometry = floorGeometry,
				.textureFiltering = textureFiltering,
				.materialIndex = descriptor.ceilingMaterialIndex,
				.luxelScale = descriptor.luxelScale,
			};

			surfaceDescriptors = {
				frontWallDescriptor,
				rightWallDescriptor,
				frontWallDescriptor,
				rightWallDescriptor,
				floorDescriptor,
				ceilingDescriptor,
			};
		}

		vec3<float32> frontWallTranslation;
	};

	template<>
	inline void InteriorSceneBuilder::addRoom(RectangularRoom& room) {
		const uint32 frontWallIndex = placeSurface(
			room.surfaceDescriptors[0],
			{
				.translation = room.frontWallTranslation,
			}
		);

		const uint32 rightWallIndex = snapSurface(room.surfaceDescriptors[1], frontWallIndex, 1, 3, radians(90.0_deg));

		snapSurface(room.surfaceDescriptors[2], rightWallIndex, 1, 3, radians(180.0_deg));
		snapSurface(room.surfaceDescriptors[3], frontWallIndex, 3, 1, radians(90.0_deg));
		snapSurface(room.surfaceDescriptors[4], frontWallIndex, 2, 0, radians(90.0_deg));
		snapSurface(room.surfaceDescriptors[5], frontWallIndex, 0, 2, radians(90.0_deg));
	}
}