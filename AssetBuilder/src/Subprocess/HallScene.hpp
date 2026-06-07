// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	inline Scene3DData createHallScene(std::filesystem::path rootOutputPath) {
		StaticSceneBuilder builder(rootOutputPath);
		builder
			.createStaticObject({
				.label = "hall_floor",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {56.0f, 0.0f, 84.0f},
					.scale = {144.0f, 0.0f, 200.0f},
				},
				.textureFiltering = {
					.translation = {0.25f, 0.625f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			/* .createStaticObject({
				.label = "hall_to_bike_room_floor_connector",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-20.0f, 0.0f, 128.0f},
					.scale = {8.0f, 0.0f, 48.0f},
				},
				.textureFiltering = {
					.translation = {0.375f, 0.125f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "bike_room_floor",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-60.0f, 0.0f, 116.0f},
					.scale = {72.0f, 0.0f, 136.0f},
				},
				.textureFiltering = {
					.translation = {0.5f, 0.624f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "hall_to_main_floor_connector",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {54.0f, 0.0f, 188.0f},
					.scale = {60.0f, 0.0f, 8.0f},
				},
				.textureFiltering = {
					.translation = {0.625f, 0.5f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "main_floor",
				.geometryAssetName = "UnitPlane",
				.transform = {
					.position = {-20.0f, 0.0f, 256.0f},
					.scale = {296.0f, 0.0f, 128.0f},
				},
				.textureFiltering = {
					.translation = {0.625f, 0.5f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.lightmapPixelScale = {1.0f, 1.0f},
			}) */;

		// TODO(test)
		builder
			.createStaticObject({
				.label = "test_box",
				.geometryAssetName = "UnitBox",
				.transform = {
					.position = {56.0f, 24.0f, 84.0f},
					.scale = {48.0f, 32.0f, 16.0f},
				},
				.textureFiltering = {
					.translation = {0.25f, 0.625f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.lightmapPixelScale = {1.0f, 1.0f},
			})
			.createStaticObject({
				.label = "test_box_2",
				.geometryAssetName = "UnitBox",
				.transform = {
					.position = {128.0f, 46.0f, 84.0f},
					.scale = {128.0f, 8.0f, 256.0f},
				},
				.textureFiltering = {
					.translation = {0.25f, 0.625f},
					.rotation = 0.0f,
					.scale = {-0.25f, 0.25f},
				},
				.lightmapPixelScale = {1.0f, 1.0f},
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
						.position = {0.0f, 0.0f, 0.0f},
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