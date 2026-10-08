// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/LinearMap/Builder/LinearMapBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class HallMap final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_hall";
			}

			MapBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr vec2<float32> LUXEL_SCALE = {4.0f, 4.0f};
				static constexpr vec3<float32> HALL_ENTRANCE_CARVING_VERTICES[] = {
					{ 92.0f,  112.0f, -16.0f },
					{ 36.0f,  112.0f, -16.0f },
					{ 36.0f,  4.0f,   -16.0f },
					{ 92.0f,  4.0f,   -16.0f },
				};
				static constexpr vec3<float32> MAIN_ENTRANCE_CARVING_VERTICES[] = {
					{ 24.0f,  112.0f,  184.0f},
					{ 84.0f,  112.0f,  184.0f},
					{ 84.0f,  4.0f,    184.0f},
					{ 24.0f,  4.0f,    184.0f},
				};
				static constexpr vec3<float32> HALL_ROOM_TO_BIKE_ROOM_CARVING_VERTICES[] = {
					{-16.0f,  112.0f,  100.0f},
					{-16.0f,  112.0f,  156.0f},
					{-16.0f,  4.0f,    156.0f},
					{-16.0f,  4.0f,    100.0f},
				};
				static constexpr vec3<float32> BIKE_ROOM_TO_HALL_ROOM_CARVING_VERTICES[] = {
					{-24.0f,  112.0f,  156.0f},
					{-24.0f,  112.0f,  100.0f},
					{-24.0f,  4.0f,    100.0f},
					{-24.0f,  4.0f,    156.0f},
				};

				LinearMapBuilder builder({
					.skyColor = {0.0f, 0.0f, 0.0f},
					.environmentMapImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_512p"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {100.0f, 0.0f, 32.0f},
						.direction = normalized(vec3<float32>{radians(-40.0_deg), 0.0f, radians(50.0_deg)}),
					},
				});

				const MaterialReference material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));

				Room& hallRoom = builder.createRectangularRoom({
					.origin = {56.0f, 0.0f, 84.0f},
					.size = {144.0f, 240.0f, 200.0f},
					.material = material,
					.textureFiltering = {
						.translation = {0.25f, 0.625f},
						.rotation = 0.0f,
						.scale = {-0.25f * 256.0f, 0.25f * 256.0f},
					},
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = {1.0f, 1.0f, 1.0f},
					},
					/* .floorMaterialIndex = tileFloorMaterialIndex,
					.wallMaterialIndex = plasterWallMateralIndex,
					.ceilingMaterialIndex = plasterCeilingMaterialIndex, */
				});

				Room& bikeRoom = builder.createRectangularRoom({
					.origin = {-60.0f, 0.0f, 116.0f},
					.size = {72.0f, 128.0f, 136.0f},
					.material = material,
					.textureFiltering = {
						.translation = {0.5f, 0.624f},
						.rotation = 0.0f,
						.scale = {-0.25f * 256.0f, 0.25f * 256.0f},
					},
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = {1.0f, 1.0f, 1.0f},
					},
					/* .floorMaterialIndex = tileFloorMaterialIndex,
					.wallMaterialIndex = plasterWallMaterialIndex,
					.ceilingMaterialIndex = plasterCeilingMaterialIndex, */
				});

				{
					Face& hallRoomBackWallFace = builder.getFace(hallRoom.faceIndices[RectangularRoomDescriptor::BACK_WALL_FACE]);

					builder.carveFace(hallRoomBackWallFace, HALL_ENTRANCE_CARVING_VERTICES);
					builder.removeFace(builder.getLastFaceIndex());
				}

				{
					Face& hallRoomFrontWallFace = builder.getFace(hallRoom.faceIndices[RectangularRoomDescriptor::FRONT_WALL_FACE]);

					builder.carveFace(hallRoomFrontWallFace, MAIN_ENTRANCE_CARVING_VERTICES);
					builder.removeFace(builder.getLastFaceIndex());
				}

				{
					Face& hallRoomToBikeRoomFace = builder.getFace(hallRoom.faceIndices[RectangularRoomDescriptor::LEFT_WALL_FACE]);
					Face& bikeRoomToHallRoomFace = builder.getFace(bikeRoom.faceIndices[RectangularRoomDescriptor::RIGHT_WALL_FACE]);

					builder.connectFaces(hallRoomToBikeRoomFace, bikeRoomToHallRoomFace, HALL_ROOM_TO_BIKE_ROOM_CARVING_VERTICES, BIKE_ROOM_TO_HALL_ROOM_CARVING_VERTICES);
				}

				// TODO(asset-builder/map): Implement.
				/* WindowFrame hallRoomDoorFrame({
					.room = &hallRoom,
					.floorOffset = 0.0f,
					.horizontalOffset = 52.0f,
					.size = {56.0f, 112.0f},
				});
				WindowFrame mainRoomDoorFrame({
					.room = &hallRoom,
					.floorOffset = 0.0f,
					.horizontalOffset = 36.0f,
					.size = {68.0f, 112.0f},
				});
				DoorFrame bikeRoomDoorFrame({
					.roomA = &hallRoom,
					.horizontalOffsetA = 116.0f,
					.roomB = &bikeRoom,
					.horizontalOffsetB = 28.0f,
					.doorSize = {48.0f, 108.0f},
					.frameThickness = 4.0f,
					.frameMaterialIndex = woodDoorMaterialIndex,
					.frameLuxelScale = {4.0f, 4.0f},
				}); */

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}