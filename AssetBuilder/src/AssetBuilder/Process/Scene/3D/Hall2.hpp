// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/LinearMapBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class Hall2Scene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_hall2";
			}

			SceneBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				LinearMapBuilder builder({
					.environmentMapImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_512p"),
					.geometryAssets = {},
					.staticMaterialAssets = {
						assetLibrary->getAsset<MaterialAsset>("lm_default"),
					},
					.dynamicMaterialAssets = {},
				});

				const uint32 hallRoomIndex = builder.createRectangularRoom({
					.origin = {56.0f, 0.0f, 84.0f},
					.size = {144.0f, 240.0f, 200.0f},
					/* .luxelScale = {4.0f, 4.0f},
					.floorMaterialIndex = tileFloorMaterialIndex,
					.wallMaterialIndex = plasterWallMateralIndex,
					.ceilingMaterialIndex = plasterCeilingMaterialIndex,
					.floorTextureFiltering = {
						.translation = {0.25f, 0.625f},
						.rotation = 0.0f,
						.scale = {-0.25f * 256.0f, 0.25f * 256.0f},
					}, */
				});

				const uint32 bikeRoomIndex = builder.createRectangularRoom({
					.origin = {-60.0f, 0.0f, 116.0f},
					.size = {72.0f, 128.0f, 136.0f},
					/* .luxelScale = {4.0f, 4.0f},
					.floorMaterialIndex = tileFloorMaterialIndex,
					.wallMaterialIndex = plasterWallMaterialIndex,
					.ceilingMaterialIndex = plasterCeilingMaterialIndex,
					.floorTextureFiltering = {
						.translation = {0.5f, 0.624f},
						.rotation = 0.0f,
						.scale = {-0.25f * 256.0f, 0.25f * 256.0f},
					}, */
				});

				// TODO(asset-builder/map): Implement.
				/* WindowFrame hallRoomDoorFrame({
					.room = &hallRoom,
					.surfaceIndex = 2,
					.floorOffset = 0.0f,
					.horizontalOffset = 52.0f,
					.size = {56.0f, 112.0f},
				});
				DoorFrame bikeRoomDoorFrame({
					.roomA = &hallRoom,
					.surfaceIndexA = 3,
					.horizontalOffsetA = 116.0f,

					.roomB = &bikeRoom,
					.surfaceIndexB = 1,
					.horizontalOffsetB = 28.0f,

					.doorSize = {48.0f, 108.0f},
					.frameThickness = 4.0f,
					.frameMaterialIndex = woodDoorMaterialIndex,
					.frameLuxelScale = {4.0f, 4.0f},
				});
				WindowFrame mainRoomDoorFrame({
					.room = &hallRoom,
					.surfaceIndex = 0,
					.floorOffset = 0.0f,
					.horizontalOffset = 36.0f,
					.size = {68.0f, 112.0f},
				});

				builder.addRoom(hallRoom);
				builder.addRoom(bikeRoom);
				builder.addFrame(hallRoomDoorFrame);
				builder.addFrame(bikeRoomDoorFrame);
				builder.addFrame(mainRoomDoorFrame); */

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}