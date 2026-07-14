// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

namespace Trident {
	inline Scene3DData createHallScene(std::filesystem::path rootOutputPath) {
		static constexpr uint32 MATERIAL_IDENTIFIER_OFFSET = 100;
		static constexpr uint32 floorMaterialIndex = 0;
		static constexpr uint32 wallMaterialIndex = 1;
		static constexpr uint32 doorMaterialIndex = 2;
		static constexpr uint32 ceilingMaterialIndex = 3;

		// Texture scale multipliers
		static constexpr float32 m1 = 1.0f / 256.0f;
		static constexpr float32 m2 = 1.0f / 512.0f;

		const vec3<float32> upRotation = {0.0f, 0.0f, 0.0f};
		const vec3<float32> downRotation = {0.0f, 0.0f, pi<float32>()};
		const vec3<float32> leftRotation = {0.0f, -pi<float32>() * 0.5f, -pi<float32>() * 0.5f};
		const vec3<float32> rightRotation = {0.0f, pi<float32>() * 0.5f, pi<float32>() * 0.5f};
		const vec3<float32> forwardRotation = {-pi<float32>() * 0.5f, 0.0f, pi<float32>()};
		const vec3<float32> backwardRotation = {pi<float32>() * 0.5f, 0.0f, 0.0f};

		StaticSceneBuilder builder(rootOutputPath, "Hall");

		builder.addMaterialShader(
			"Lightmapped.frag",
			ShaderStage::FRAGMENT,
			"main",
			{
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
			}
		);

		builder
			.addMaterial(MATERIAL_IDENTIFIER_OFFSET + floorMaterialIndex, 0)
			.addMaterial(MATERIAL_IDENTIFIER_OFFSET + wallMaterialIndex, 0)
			.addMaterial(MATERIAL_IDENTIFIER_OFFSET + doorMaterialIndex, 0)
			.addMaterial(MATERIAL_IDENTIFIER_OFFSET + ceilingMaterialIndex, 0);

		// Floors
		builder
			.createStaticObject({
				.label = "hall_floor",
				.transform = {
					.position = {56.0f, 0.0f, 84.0f},
					.rotation = upRotation,
					.scale = {144.0f, 0.0f, 200.0f},
				},
				.textureFiltering = {
					.translation = {0.25f, 0.625f},
					.rotation = 0.0f,
					.scale = {m1 / -0.25f, m1 / 0.25f},
				},
				.materialIndex = floorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "hall_to_bike_room_floor_connector",
				.transform = {
					.position = {-20.0f, 0.0f, 128.0f},
					.rotation = upRotation,
					.scale = {8.0f, 0.0f, 48.0f},
				},
				.textureFiltering = {
					.translation = {0.375f, 0.125f},
					.rotation = 0.0f,
					.scale = {m1 / -0.25f, m1 / 0.25f},
				},
				.materialIndex = floorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_floor",
				.transform = {
					.position = {-60.0f, 0.0f, 116.0f},
					.rotation = upRotation,
					.scale = {72.0f, 0.0f, 136.0f},
				},
				.textureFiltering = {
					.translation = {0.5f, 0.624f},
					.rotation = 0.0f,
					.scale = {m1 / -0.25f, m1 / 0.25f},
				},
				.materialIndex = floorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "hall_to_main_floor_connector",
				.transform = {
					.position = {54.0f, 0.0f, 188.0f},
					.rotation = upRotation,
					.scale = {60.0f, 0.0f, 8.0f},
				},
				.textureFiltering = {
					.translation = {0.625f, 0.5f},
					.rotation = 0.0f,
					.scale = {m1 / -0.25f, m1 / 0.25f},
				},
				.materialIndex = floorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "main_floor",
				.transform = {
					.position = {-20.0f, 0.0f, 256.0f},
					.rotation = upRotation,
					.scale = {296.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.625f, 0.5f},
					.rotation = 0.0f,
					.scale = {m1 / -0.25f, m1 / 0.25f},
				},
				.materialIndex = floorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			});

		// Walls
		builder
			.createStaticObject({
				.label = "hall_entrance_left_wall",
				.transform = {
					.position = {10.0f, 64.0f, -16.0f},
					.rotation = forwardRotation,
					.scale = {52.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.281f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "hall_entrance_right_wall",
				.transform = {
					.position = {110.0f, 64.0f, -16.0f},
					.rotation = forwardRotation,
					.scale = {36.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.0f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_entrance_left_wall",
				.transform = {
					.position = {-16.0f, 64.0f, 42.0f},
					.rotation = rightRotation,
					.scale = {116.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.39f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / 0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_entrance_right_wall",
				.transform = {
					.position = {-16.0f, 64.0f, 170.0f},
					.rotation = rightRotation,
					.scale = {28.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.36f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / 0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "main_entrance_left_wall",
				.transform = {
					.position = {2.0f, 64.0f, 184.0f},
					.rotation = backwardRotation,
					.scale = {36.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.125f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "main_entrance_right_wall",
				.transform = {
					.position = {108.0f, 64.0f, 184.0f},
					.rotation = backwardRotation,
					.scale = {40.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.312f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "mailbox_wall",
				.transform = {
					.position = {128.0f, 64.0f, 84.0f},
					.rotation = leftRotation,
					.scale = {200.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.262f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_left_wall",
				.transform = {
					.position = {-60.0f, 64.0f, 48.0f},
					.rotation = forwardRotation,
					.scale = {72.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.812f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_back_wall",
				.transform = {
					.position = {-96.0f, 64.0f, 116.0f},
					.rotation = rightRotation,
					.scale = {136.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.625f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / 0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_right_wall",
				.transform = {
					.position = {-60.0f, 64.0f, 184.0f},
					.rotation = backwardRotation,
					.scale = {72.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.75f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_entrance_left_wall",
				.transform = {
					.position = {-24.0f, 64.0f, 170.0f},
					.rotation = leftRotation,
					.scale = {28.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.58f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_entrance_right_wall",
				.transform = {
					.position = {-24.0f, 64.0f, 74.0f},
					.rotation = leftRotation,
					.scale = {52.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.295f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "hall_entrance_door_wall",
				.transform = {
					.position = {64.0f, 120.0f, -16.0f},
					.rotation = forwardRotation,
					.scale = {56.0f, 0.0f, 16.0f},
				},
				.textureFiltering = {
					.translation = {0.71f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_entrance_door_wall",
				.transform = {
					.position = {-16.0f, 120.0f, 128.0f},
					.rotation = rightRotation,
					.scale = {56.0f, 0.0f, 16.0f},
				},
				.textureFiltering = {
					.translation = {0.36f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / 0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "main_entrance_door_wall",
				.transform = {
					.position = {54.0f, 120.0f, 184.0f},
					.rotation = backwardRotation,
					.scale = {68.0f, 0.0f, 16.0f},
				},
				.textureFiltering = {
					.translation = {0.53f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_entrance_door_wall",
				.transform = {
					.position = {-24.0f, 120.0f, 128.0f},
					.rotation = leftRotation,
					.scale = {56.0f, 0.0f, 16.0f},
				},
				.textureFiltering = {
					.translation = {0.797f, 0.0f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = wallMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			});

		// Door frames
		builder
			.createStaticObject({
				.label = "bike_room_entrance_door_frame_left",
				.transform = {
					.position = {-16.0f, 54.0f, 102.0f},
					.rotation = rightRotation,
					.scale = {4.0f, 0.0f, 108.0f},
				},
				.textureFiltering = {
					.translation = {0.875f, 0.155f},
					.rotation = 0.0f,
					.scale = {m1 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = doorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_entrance_door_frame_right",
				.transform = {
					.position = {-16.0f, 54.0f, 154.0f},
					.rotation = rightRotation,
					.scale = {4.0f, 0.0f, 108.0f},
				},
				.textureFiltering = {
					.translation = {0.0625f, 0.155f},
					.rotation = 0.0f,
					.scale = {m1 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = doorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_entrance_door_frame_top",
				.transform = {
					.position = {-16.0f, 110.0f, 128.0f},
					.rotation = rightRotation,
					.scale = {56.0f, 0.0f, 4.0f},
				},
				.textureFiltering = {
					.translation = {-0.125f, 0.125f},
					.rotation = 0.0f,
					.scale = {m1 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = doorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_entrance_door_frame_left_inside",
				.transform = {
					.position = {-20.0f, 54.0f, 104.0f},
					.rotation = forwardRotation,
					.scale = {8.0f, 0.0f, 108.0f},
				},
				.textureFiltering = {
					.translation = {0.195f, 0.1575f},
					.rotation = 0.0f,
					.scale = {m1 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = doorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_entrance_door_frame_right_inside",
				.transform = {
					.position = {-20.0f, 54.0f, 152.0f},
					.rotation = backwardRotation,
					.scale = {8.0f, 0.0f, 108.0f},
				},
				.textureFiltering = {
					.translation = {0.0625f, 0.1575f},
					.rotation = 0.0f,
					.scale = {m1 / 0.25f, m2 / 0.25f},
				},
				.materialIndex = doorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_entrance_door_frame_top_inside",
				.transform = {
					.position = {-20.0f, 108.0f, 128.0f},
					.rotation = downRotation,
					.scale = {8.0f, 0.0f, 48.0f},
				},
				.textureFiltering = {
					.translation = {0.19f, -0.017f},
					.rotation = 1.0f,
					.scale = {m1 / 0.25f, m2 / 0.25f},
				},
				.materialIndex = doorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_entrance_door_frame_left",
				.transform = {
					.position = {-24.0f, 54.0f, 154.0f},
					.rotation = leftRotation,
					.scale = {4.0f, 0.0f, 108.0f},
				},
				.textureFiltering = {
					.translation = {0.0f, 0.155f},
					.rotation = 0.0f,
					.scale = {m1 / 0.25f, m2 / 0.25f},
				},
				.materialIndex = doorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_entrance_door_frame_right",
				.transform = {
					.position = {-24.0f, 54.0f, 102.0f},
					.rotation = leftRotation,
					.scale = {4.0f, 0.0f, 108.0f},
				},
				.textureFiltering = {
					.translation = {0.8125f, 0.155f},
					.rotation = 0.0f,
					.scale = {m1 / 0.25f, m2 / 0.25f},
				},
				.materialIndex = doorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_entrance_door_frame_top",
				.transform = {
					.position = {-24.0f, 110.0f, 128.0f},
					.rotation = leftRotation,
					.scale = {56.0f, 0.0f, 4.0f},
				},
				.textureFiltering = {
					.translation = {0.0f, 0.125f},
					.rotation = 0.0f,
					.scale = {m1 / 0.25f, m2 / 0.25f},
				},
				.materialIndex = doorMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			});

		// Ceiling
		builder
			.createStaticObject({
				.label = "hall_entrance_top_wall",
				.transform = {
					.position = {56.0f, 184.0f, -16.0f},
					.rotation = forwardRotation,
					.scale = {144.0f, 0.0f, 112.0f},
				},
				.textureFiltering = {
					.translation = {0.0f, 0.125f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = ceilingMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_entrance_top_wall",
				.transform = {
					.position = {-16.0f, 184.0f, 84.0f},
					.rotation = rightRotation,
					.scale = {200.0f, 0.0f, 112.0f},
				},
				.textureFiltering = {
					.translation = {0.125f, 0.125f},
					.rotation = 0.0f,
					.scale = {m2 / 0.25f, m2 / 0.25f},
				},
				.materialIndex = ceilingMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "main_entrance_top_wall",
				.transform = {
					.position = {56.0f, 184.0f, 184.0f},
					.rotation = backwardRotation,
					.scale = {144.0f, 0.0f, 112.0f},
				},
				.textureFiltering = {
					.translation = {0.125f, 0.125f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = ceilingMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "mailbox_top_wall",
				.transform = {
					.position = {128.0f, 184.0f, 84.0f},
					.rotation = leftRotation,
					.scale = {200.0f, 0.0f, 112.0f},
				},
				.textureFiltering = {
					.translation = {0.686f, 0.125f},
					.rotation = 0.0f,
					.scale = {m2 / -0.25f, m2 / 0.25f},
				},
				.materialIndex = ceilingMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "hall_ceiling",
				.transform = {
					.position = {56.0f, 240.0f, 84.0f},
					.rotation = downRotation,
					.scale = {144.0f, 0.0f, 200.0f},
				},
				.textureFiltering = {
					.translation = {0.498f, 0.875f},
					.rotation = 0.5f,
					.scale = {m2 / 0.25f, m2 / -0.25f},
				},
				.materialIndex = ceilingMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			})
			.createStaticObject({
				.label = "bike_room_ceiling",
				.transform = {
					.position = {-60.0f, 128.0f, 116.0f},
					.rotation = downRotation,
					.scale = {72.0f, 0.0f, 136.0f},
				},
				.textureFiltering = {
					.translation = {0.69f, 0.258f},
					.rotation = 0.5f,
					.scale = {m2 / 0.25f, m2 / -0.25f},
				},
				.materialIndex = ceilingMaterialIndex,
				.lightmapPixelScale = {4.0f, 4.0f},
			});

		return {
			.staticScene = builder.build(),
			.entityCount = 1,
			.geometryAssetIdentifiers = {
				// TODO(debug-only): UnitBox
				38,
			},
			.geometryMappings = {
				{0, {0}},
			},
			.materialMappings = {
				// NOTE: Counting default material for visibility rendering.
				{0, {1}},
			},
			.rigidBodyMappings = {
				// Player
				{
					0,
					createRigidBody({
						.position = {20.0f, 77.0f, 20.0f},
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
}