// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"

namespace Trident {
	inline Scene3DData createCabinScene(std::filesystem::path rootOutputPath) {
		InteriorSceneBuilder builder(rootOutputPath, "Cabin");

		builder.floorLevels = {
			0.0f, // Floor
			128.0f, // Bike room ceiling
			240.0f, // Hall/main room ceiling
		};

		const RectangularRoom hallRoom(
			{56.0f, 84.0f},
			{144.0f, 200.0f},
			builder.floorLevels[0],
			builder.floorLevels[2],
			{4.0f, 4.0f}
		);

		const RectangularRoom bikeRoom(
			{-60.0f, 116.0f},
			{72.0f, 136.0f},
			builder.floorLevels[0],
			builder.floorLevels[1],
			{4.0f, 4.0f}
		);

		const RectangularRoom mainRoom(
			{-20.0f, 256.0f},
			{296.0f, 128.0f},
			builder.floorLevels[0],
			builder.floorLevels[2],
			{4.0f, 4.0f}
		);

		// builder.addRoom(0, 128.0f, 96.0f);
		builder.addRoom(hallRoom);
		builder.addRoom(bikeRoom);
		builder.addRoom(mainRoom);

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