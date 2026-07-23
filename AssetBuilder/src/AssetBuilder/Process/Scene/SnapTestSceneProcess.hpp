// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Geometry/StaticGeometry3D.hpp"
#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"
#include "../../../Subprocess/Hull.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class SnapTestSceneProcess final : public Scene3DProcess {
		private:
			static RectangularRoom createRoom(svec2<float32> position, const float32* floorLevel, uint32 floorMaterialIndex, uint32 wallMaterialIndex, uint32 ceilingMaterialIndex) {
				return RectangularRoom({
					.position = position,
					.size = {200.0f, 200.0f},
					.floorLevel = floorLevel[0],
					.ceilingLevel = floorLevel[1],
					.luxelScale = {4.0f, 4.0f},
					.floorMaterialIndex = floorMaterialIndex,
					.wallMaterialIndex = wallMaterialIndex,
					.ceilingMaterialIndex = ceilingMaterialIndex,
					.floorTextureFiltering = {
						.translation = {0.25f, 0.625f},
						.rotation = 0.0f,
						.scale = {-0.25f * 256.0f, 0.25f * 256.0f},
					},
				});
			}

			static DoorFrame createFrame(RectangularRoom* roomA, RectangularRoom* roomB, uint32 surfaceIndexA, uint32 surfaceIndexB, float32 doorWidth, float32 horizontalOffset, uint32 frameMaterialIndex) {
				return DoorFrame({
					.roomA = roomA,
					.surfaceIndexA = surfaceIndexA,
					.horizontalOffsetA = horizontalOffset,
					.roomB = roomB,
					.surfaceIndexB = surfaceIndexB,
					.horizontalOffsetB = horizontalOffset,
					.doorSize = {doorWidth, 108.0f},
					.frameThickness = 4.0f,
					.frameMaterialIndex = frameMaterialIndex,
					.frameLuxelScale = {4.0f, 4.0f},
				});
			}

			static DoorFrame createCenteredFrame(RectangularRoom* roomA, RectangularRoom* roomB, uint32 surfaceIndexA, uint32 surfaceIndexB, float32 doorWidth, uint32 frameMaterialIndex) {
				const float32 horizontalOffset = (200.0f - (doorWidth + 8.0f)) * 0.5f;

				return createFrame(roomA, roomB, surfaceIndexA, surfaceIndexB, doorWidth, horizontalOffset, frameMaterialIndex);
			}

		protected:
			view<char8> getAssetName() const final override {
				return "SnapTest";
			}

			Scene3DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				InteriorSceneBuilder builder;
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

				const uint32 plasterCeilingMaterialIndex = builder.addMaterial(assetLibrary->getAsset<MaterialAsset>("plasterceiling"));
				const uint32 plasterWallMaterialIndex = builder.addMaterial(assetLibrary->getAsset<MaterialAsset>("plasterwall"));
				const uint32 tileFloorMaterialIndex = builder.addMaterial(assetLibrary->getAsset<MaterialAsset>("tilefloor"));
				const uint32 woodDoorMaterialIndex = builder.addMaterial(assetLibrary->getAsset<MaterialAsset>("wooddoor"));

				builder.floorLevels = {
					0.0f, // Floor 1
					128.0f, // Ceiling 1
					136.0f, // Floor 2
					264.0f, // Ceiling 2
				};

				RectangularRoom room0 = createRoom({0.0f, 0.0f}, &builder.floorLevels[0], tileFloorMaterialIndex, plasterWallMaterialIndex, plasterCeilingMaterialIndex);
				RectangularRoom room1 = createRoom({0.0f, 208.0f}, &builder.floorLevels[0], tileFloorMaterialIndex, plasterWallMaterialIndex, plasterCeilingMaterialIndex);
				RectangularRoom room2 = createRoom({208.0f, 0.0f}, &builder.floorLevels[0], tileFloorMaterialIndex, plasterWallMaterialIndex, plasterCeilingMaterialIndex);
				RectangularRoom room3 = createRoom({0.0f, -208.0f}, &builder.floorLevels[0], tileFloorMaterialIndex, plasterWallMaterialIndex, plasterCeilingMaterialIndex);
				RectangularRoom room4 = createRoom({-208.0f, 0.0f}, &builder.floorLevels[0], tileFloorMaterialIndex, plasterWallMaterialIndex, plasterCeilingMaterialIndex);
				// RectangularRoom room5 = createRoom({0.0f, 208.0f}, &builder.floorLevels[2], tileFloorMaterialIndex, plasterWallMaterialIndex, plasterCeilingMaterialIndex);

				room0.surfaceDescriptors[room0.CEILING].carvings.emplace_back(Carving{
					.offset = {32.0f, 32.0f},
					.size = {136.0f, 136.0f},
				});

				DoorFrame frame01 = createCenteredFrame(&room0, &room1, 0, 2, 48.0f, woodDoorMaterialIndex);
				DoorFrame frame02 = createCenteredFrame(&room0, &room2, 1, 3, 48.0f, woodDoorMaterialIndex);
				DoorFrame frame03 = createCenteredFrame(&room0, &room3, 2, 0, 48.0f, woodDoorMaterialIndex);
				DoorFrame frame04 = createCenteredFrame(&room0, &room4, 3, 1, 48.0f, woodDoorMaterialIndex);

				builder.addRoom(room0);
				builder.addRoom(room1);
				builder.addRoom(room2);
				builder.addRoom(room3);
				builder.addRoom(room4);
				// builder.addRoom(room5);

				builder.addFrame(frame01);
				builder.addFrame(frame02);
				builder.addFrame(frame03);
				builder.addFrame(frame04);

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
								.position = {0.0f, 77.0f, 0.0f},
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
			explicit SnapTestSceneProcess(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}