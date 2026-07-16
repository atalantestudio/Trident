// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"

namespace Trident {
	inline Scene3DData createCabinScene(std::filesystem::path rootOutputPath) {
		static constexpr uint32 materialCount = 2;
		static constexpr uint32 defaultMaterialIndex = 0;
		static constexpr uint32 lightMaterialIndex = 1;

		InteriorSceneBuilder builder(rootOutputPath, "Cabin");

		const uint32 floorLevel0 = builder.addFloorLevel(0.0f);
		const uint32 floorLevel1 = builder.addFloorLevel(128.0f);

		// builder.addRoom(floorLevel0, 128.0f, 96.0f);

		// TODO(test)
		StaticSurface surface = {
			.materialIndex = 0,
			.transform = {
				.position = {0.0f, 16.0f, 144.0f},
				.rotation = {pi<float32>() * 0.5f, 0.0f, 0.0f},
				.scale = {200.0f, 100.0f},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {(1.0f / 256.0f) / 0.25f, (1.0f / 256.0f) / 0.25f},
			},
			.lightmapScale = {4.0f, 4.0f},
		};

		surface.carvings = {
			{
				.offset = {20.0f, 20.0f},
				.scale = {60.0f, 60.0f},
			},
			{
				.offset = {100.0f, 0.0f},
				.scale = {20.0f, 50.0f},
			},
		};

		builder.addSurface(surface);

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
						.position = {0.0f, 16.5f, 0.0f},
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