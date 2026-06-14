// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

namespace Trident {
	inline Scene3DData createHallScene(std::filesystem::path rootOutputPath) {
		static constexpr uint32 materialCount = 2;
		static constexpr uint32 defaultMaterialIndex = 0;
		static constexpr uint32 lightMaterialIndex = 1;

		StaticSceneBuilder builder(rootOutputPath);

		builder
			.allocateMaterials(materialCount)
			.insertMaterial(defaultMaterialIndex, {
				.albedo = {0.8f, 0.8f, 0.8f, 1.0f},
				.emissiveColor = {0.0f, 0.0f, 0.0f, 1.0f},
				.emissiveStrength = 0.0f,
			})
			.insertMaterial(lightMaterialIndex, {
				.albedo = {0.0f, 0.0f, 0.0f, 1.0f},
				.emissiveColor = {1.0f, 1.0f, 1.0f, 1.0f},
				.emissiveStrength = 16.0f,
			});

		builder
			.createStaticObject({
				.label = "hall_floor",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {56.0f, 0.0f, 84.0f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.scale = {144.0f, 0.0f, 200.0f},
				},
				.textureFiltering = {
					.translation = {0.25f, 0.625f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "hall_to_bike_room_floor_connector",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-20.0f, 0.0f, 128.0f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.scale = {8.0f, 0.0f, 48.0f},
				},
				.textureFiltering = {
					.translation = {0.375f, 0.125f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "bike_room_floor",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-60.0f, 0.0f, 116.0f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.scale = {72.0f, 0.0f, 136.0f},
				},
				.textureFiltering = {
					.translation = {0.5f, 0.624f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "hall_to_main_floor_connector",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {54.0f, 0.0f, 188.0f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.scale = {60.0f, 0.0f, 8.0f},
				},
				.textureFiltering = {
					.translation = {0.625f, 0.5f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "main_floor",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-20.0f, 0.0f, 256.0f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.scale = {296.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.625f, 0.5f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			});

		// Walls
		builder
			.createStaticObject({
				.label = "hall_entrance_left_wall",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {10.0f, 64.0f, -16.0f},
					.rotation = {-pi<float32>() / 2.0f, 0.0f, 0.0f},
					.scale = {52.0f, 128.0f, 0.0f},
				},
				.textureFiltering = {
					.translation = {0.281f, 0.0f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "hall_entrance_right_wall",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {110.0f, 64.0f, -16.0f},
					.rotation = {-pi<float32>() / 2.0f, 0.0f, 0.0f},
					.scale = {36.0f, 128.0f, 0.0f},
				},
				.textureFiltering = {
					.translation = {0.0f, 0.0f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "bike_room_entrance_left_wall",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-16.0f, 64.0f, 42.0f},
					.rotation = {0.0f, 0.0f, pi<float32>() / 2.0f},
					.scale = {0.0f, 128.0f, 116.0f},
				},
				.textureFiltering = {
					.translation = {0.39f, 0.0f},
					.rotation = 0.0f,
					.scale = {0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "bike_room_entrance_right_wall",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-16.0f, 64.0f, 170.0f},
					.rotation = {0.0f, 0.0f, pi<float32>() / 2.0f},
					.scale = {0.0f, 128.0f, 28.0f},
				},
				.textureFiltering = {
					.translation = {0.36f, 0.0f},
					.rotation = 0.0f,
					.scale = {0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "main_entrance_left_wall",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {2.0f, 64.0f, 184.0f},
					.rotation = {pi<float32>() / 2.0f, 0.0f, 0.0f},
					.scale = {36.0f, 128.0f, 0.0f},
				},
				.textureFiltering = {
					.translation = {0.125f, 0.0f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "main_entrance_right_wall",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {108.0f, 64.0f, 184.0f},
					.rotation = {pi<float32>() / 2.0f, 0.0f, 0.0f},
					.scale = {40.0f, 128.0f, 0.0f},
				},
				.textureFiltering = {
					.translation = {0.312f, 0.0f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "mailbox_wall",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {128.0f, 64.0f, 84.0f},
					.rotation = {0.0f, 0.0f, -pi<float32>() / 2.0f},
					.scale = {0.0f, 128.0f, 200.0f},
				},
				.textureFiltering = {
					.translation = {0.262f, 0.0f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_left_wall",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-60.0f, 64.0f, 48.0f},
					.rotation = {-pi<float32>() / 2.0f, 0.0f, 0.0f},
					.scale = {72.0f, 128.0f, 0.0f},
				},
				.textureFiltering = {
					.translation = {0.812f, 0.0f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_back_wall",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-96.0f, 64.0f, 116.0f},
					.rotation = {0.0f, 0.0f, pi<float32>() / 2.0f},
					.scale = {0.0f, 128.0f, 136.0f},
				},
				.textureFiltering = {
					.translation = {0.625f, 0.0f},
					.rotation = 0.0f,
					.scale = {0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_right_wall",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-60.0f, 64.0f, 184.0f},
					.rotation = {pi<float32>() / 2.0f, 0.0f, 0.0f},
					.scale = {72.0f, 128.0f, 0.0f},
				},
				.textureFiltering = {
					.translation = {0.75f, 0.0f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_entrance_left_wall",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-24.0f, 64.0f, 170.0f},
					.rotation = {0.0f, 0.0f, -pi<float32>() / 2.0f},
					.scale = {0.0f, 128.0f, 28.0f},
				},
				.textureFiltering = {
					.translation = {0.58f, 0.0f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "bike_room_inside_entrance_right_wall",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-24.0f, 64.0f, 74.0f},
					.rotation = {0.0f, 0.0f, -pi<float32>() / 2.0f},
					.scale = {0.0f, 128.0f, 52.0f},
				},
				.textureFiltering = {
					.translation = {0.295f, 0.0f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			});

		// TODO(test)
		builder
			/* .createStaticObject({
				.label = "test_light",
				.geometryAssetName = "UnitBox",
				.transform = {
					.position = {-60.0f, 64.0f, 116.0f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.scale = {4.0f, 2.0f, 4.0f},
				},
				.textureFiltering = {
					.translation = {0.0f, 0.0f},
					.rotation = 0.0f,
					.scale = {1.0f, 1.0f},
				},
				.materialIndex = lightMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			}) */
			/* .createStaticObject({
				.label = "test_box",
				.geometryAssetName = "UnitBox",
				.transform = {
					.position = {0.0f, 0.0f, 0.0f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.scale = {128.0f, 128.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.0f, 0.0f},
					.rotation = 0.0f,
					.scale = {1.0f, 1.0f},
				},
				.materialIndex = defaultMaterialIndex,
				.lightmapPixelScale = {1.0f, 1.0f},
			}) */;

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
						.position = {0.0f, 77.0f, 0.0f},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = {33.0f, 77.0f, 33.0f},
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