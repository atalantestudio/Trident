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
		TextureFilteringDescriptor floorTextureFiltering;
	};

	struct RectangularRoom : public Room {
		static constexpr uint32 FRONT_WALL = 0;
		static constexpr uint32 RIGHT_WALL = 1;
		static constexpr uint32 BACK_WALL = 2;
		static constexpr uint32 LEFT_WALL = 3;
		static constexpr uint32 FLOOR = 4;
		static constexpr uint32 CEILING = 5;

		explicit RectangularRoom(const RectangularRoomDescriptor& descriptor) :
			position(descriptor.position.x, descriptor.floorLevel, descriptor.position.y)
		{
			const float32 sizeX = descriptor.size.x;
			const float32 sizeY = descriptor.ceilingLevel - descriptor.floorLevel;
			const float32 sizeZ = descriptor.size.y;

			frontWallTranslation = {descriptor.position.x, descriptor.floorLevel + sizeY * 0.5f, descriptor.position.y + sizeZ * 0.5f};

			const StaticGeometry3D floorGeometry = InteriorSceneBuilder::createWallGeometry({sizeX, sizeZ});
			const StaticGeometry3D frontWallGeometry = InteriorSceneBuilder::createWallGeometry({sizeX, sizeY});
			const StaticGeometry3D rightWallGeometry = InteriorSceneBuilder::createWallGeometry({sizeZ, sizeY});

			// TODO(asset-builder/scene/interior/room): Expose properties per individual room surface.
			const TextureFilteringDescriptor textureFiltering = {
				.scale = {0.25f * 512.0f, 0.25f * 512.0f},
			};

			const SurfaceDescriptor frontWallDescriptor = {
				.geometry = frontWallGeometry,
				.textureFiltering = textureFiltering,
				.materialIndex = descriptor.wallMaterialIndex,
				.lightmap = {
					.luxelScale = descriptor.luxelScale,
					.tintColor = {1.0f, 1.0f, 1.0f},
				},
			};
			const SurfaceDescriptor rightWallDescriptor = {
				.geometry = rightWallGeometry,
				.textureFiltering = textureFiltering,
				.materialIndex = descriptor.wallMaterialIndex,
				.lightmap = {
					.luxelScale = descriptor.luxelScale,
					.tintColor = {1.0f, 1.0f, 1.0f},
				},
			};
			const SurfaceDescriptor floorDescriptor = {
				.geometry = floorGeometry,
				.textureFiltering = descriptor.floorTextureFiltering,
				.materialIndex = descriptor.floorMaterialIndex,
				.lightmap = {
					.luxelScale = descriptor.luxelScale,
					.tintColor = {1.0f, 1.0f, 1.0f},
				},
			};
			const SurfaceDescriptor ceilingDescriptor = {
				.geometry = floorGeometry,
				.textureFiltering = textureFiltering,
				.materialIndex = descriptor.ceilingMaterialIndex,
				.lightmap = {
					.luxelScale = descriptor.luxelScale,
					.tintColor = {1.0f, 1.0f, 1.0f},
				},
			};

			surfaceDescriptors = {
				frontWallDescriptor,
				rightWallDescriptor,
				frontWallDescriptor,
				rightWallDescriptor,
				floorDescriptor,
				ceilingDescriptor,
			};
			surfaceIndices = sequence<uint32>(surfaceDescriptors.count());
		}

		vec3<float32> position;
		vec3<float32> frontWallTranslation;
	};

	template<>
	inline void InteriorSceneBuilder::addRoom(RectangularRoom& room) {
		room.surfaceIndices[room.FRONT_WALL] = placeSurface(
			room.surfaceDescriptors[room.FRONT_WALL],
			{
				.translation = room.frontWallTranslation,
			}
		);

		// TODO(asset-builder/scene/interior/room): Don't copy.
		const StaticSurface frontWall = surfaces[room.surfaceIndices[room.FRONT_WALL]];

		room.surfaceIndices[room.RIGHT_WALL] = snapSurface(room.surfaceDescriptors[room.RIGHT_WALL], frontWall.getEdge(1), 3, radians(90.0_deg));
		room.surfaceIndices[room.BACK_WALL] = snapSurface(room.surfaceDescriptors[room.BACK_WALL], surfaces[room.surfaceIndices[room.RIGHT_WALL]].getEdge(1), 3, radians(180.0_deg));
		room.surfaceIndices[room.LEFT_WALL] = snapSurface(room.surfaceDescriptors[room.LEFT_WALL], frontWall.getEdge(3), 1, radians(90.0_deg));
		room.surfaceIndices[room.FLOOR] = snapSurface(room.surfaceDescriptors[room.FLOOR], frontWall.getEdge(2), 0, radians(90.0_deg));
		room.surfaceIndices[room.CEILING] = snapSurface(room.surfaceDescriptors[room.CEILING], frontWall.getEdge(0), 2, radians(90.0_deg));
	}
}