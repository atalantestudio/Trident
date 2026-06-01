// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	inline Scene3DData createHallScene(std::filesystem::path rootOutputPath) {
		// TODO(test)
		// Texture: "tile/tilefloor018a.jpg"
		// Normal map: "tile/tilefloor018a_normal.jpg"

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
			})
			.createStaticObject({
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
			});

		return {
			.staticScene = builder.build(),
			.entityCount = 0,
			.geometryAssetIdentifiers = {},
			.geometryMappings = {},
			.materialMappings = {},
			.rigidBodyMappings = {},
		};
	}
}