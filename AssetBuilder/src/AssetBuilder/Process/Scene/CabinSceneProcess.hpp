// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"
#include "AssetBuilder/API/Scene/Interior/Frame/DoorFrame.hpp"
#include "AssetBuilder/API/Scene/Interior/Frame/WindowFrame.hpp"
#include "AssetBuilder/API/Scene/Interior/Room/RectangularRoom.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class CabinSceneProcess final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "Cabin";
			}

			Scene3DData createScene(std::filesystem::path rootOutputPath) const final override {
				static constexpr uint32 MATERIAL_IDENTIFIER_OFFSET = 100;
				static constexpr uint32 floorMaterialIndex = 0;
				static constexpr uint32 wallMaterialIndex = 1;
				// static constexpr uint32 woodMaterialIndex = 2;
				// static constexpr uint32 ceilingMaterialIndex = 3;
				static constexpr uint32 ceilingMaterialIndex = 2;

				InteriorSceneBuilder builder(rootOutputPath);

				builder.addMaterialShader("Lightmapped.frag", ShaderStage::FRAGMENT, "main", {
					{
						.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::FRAGMENT,
					},
					{
						.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::FRAGMENT,
					},
				});

				builder.addMaterial(MATERIAL_IDENTIFIER_OFFSET + floorMaterialIndex, 0);
				builder.addMaterial(MATERIAL_IDENTIFIER_OFFSET + wallMaterialIndex, 0);
				// builder.addMaterial(MATERIAL_IDENTIFIER_OFFSET + woodMaterialIndex, 0);
				builder.addMaterial(MATERIAL_IDENTIFIER_OFFSET + 3, 0);

				builder.floorLevels = {
					0.0f, // Floor
					128.0f, // Bike room ceiling
					240.0f, // Hall room ceiling
				};

				RectangularRoom hallRoom({
					.position = {56.0f, 84.0f},
					.size = {144.0f, 200.0f},
					.floorLevel = builder.floorLevels[0],
					.ceilingLevel = builder.floorLevels[1],
					.luxelScale = {0.5f, 0.5f},
					.floorMaterialIndex = floorMaterialIndex,
					.wallMaterialIndex = wallMaterialIndex,
					.ceilingMaterialIndex = ceilingMaterialIndex,
				});
				RectangularRoom bikeRoom({
					.position = {-60.0f, 116.0f},
					.size = {72.0f, 136.0f},
					.floorLevel = builder.floorLevels[0],
					.ceilingLevel = builder.floorLevels[1],
					.luxelScale = {0.5f, 0.5f},
					.floorMaterialIndex = floorMaterialIndex,
					.wallMaterialIndex = wallMaterialIndex,
					.ceilingMaterialIndex = ceilingMaterialIndex,
				});

				WindowFrame hallRoomDoorFrame({
					.room = &hallRoom,
					.surfaceIndex = 2,
					.floorOffset = 0.0f,
					.horizontalOffset = 52.0f,
					.size = {56.0f, 112.0f},
				});
				DoorFrame bikeRoomDoorFrame({
					.entranceRoom = &hallRoom,
					.entranceSurfaceIndex = 3,
					.entranceFloorOffset = 0.0f,
					.entranceHorizontalOffset = 116.0f,
					.exitRoom = &bikeRoom,
					.exitSurfaceIndex = 1,
					.exitFloorOffset = 0.0f,
					.exitHorizontalOffset = 28.0f,
					.size = {56.0f, 112.0f},
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
				builder.addFrame(mainRoomDoorFrame);

				return {
					.staticScene = builder.build(),
					.entityCount = 1,
					.geometryAssetIdentifiers = {
						38,
					},
					.geometryMappings = {
						{0, {0}},
					},
					.materialMappings = {
						{0, {1}},
					},
					.rigidBodyMappings = {
						// Player
						{
							0,
							createRigidBody({
								.position = {56.0f, 77.0f, 84.0f},
								.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
								.scale = {33.0f, 77.0f, 33.0f},
								.uniformScale = 33.0f,
								.mass = 70.0_kg,
								.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},
					},
				};
			}

		public:
			explicit CabinSceneProcess(BuildAssetMap* assetMap) :
				Scene3DProcess(assetMap)
			{}
	};
}