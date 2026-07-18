// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"

namespace Trident {
	inline Scene3DData createCabinScene(std::filesystem::path rootOutputPath) {
		static constexpr uint32 MATERIAL_IDENTIFIER_OFFSET = 100;
		static constexpr uint32 floorMaterialIndex = 0;
		static constexpr uint32 wallMaterialIndex = 1;
		static constexpr uint32 ceilingMaterialIndex = 2;

		InteriorSceneBuilder builder(rootOutputPath, "Cabin");

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

		builder.addMaterial(MATERIAL_IDENTIFIER_OFFSET + floorMaterialIndex, 0);
		builder.addMaterial(MATERIAL_IDENTIFIER_OFFSET + wallMaterialIndex, 0);
		builder.addMaterial(MATERIAL_IDENTIFIER_OFFSET + 3, 0);

		builder.floorLevels = {
			0.0f, // Floor
			128.0f, // Hall/bike room ceiling
			240.0f, // Top hall room ceiling
		};

		RectangularRoom hallRoom(
			{56.0f, 84.0f},
			{144.0f, 200.0f},
			builder.floorLevels[0],
			builder.floorLevels[1],
			{2.0f, 2.0f},
			floorMaterialIndex, wallMaterialIndex, ceilingMaterialIndex
		);

		RectangularRoom bikeRoom(
			{-60.0f, 116.0f},
			{72.0f, 136.0f},
			builder.floorLevels[0],
			builder.floorLevels[1],
			{2.0f, 2.0f},
			floorMaterialIndex, wallMaterialIndex, ceilingMaterialIndex
		);

		RectangularRoom mainRoom(
			{-20.0f, 256.0f},
			{296.0f, 128.0f},
			builder.floorLevels[0],
			builder.floorLevels[2],
			{2.0f, 2.0f},
			floorMaterialIndex, wallMaterialIndex, ceilingMaterialIndex
		);

		DoorFrame bikeRoomFrame(&hallRoom, &bikeRoom, 3, 4, {116.0f, 16.0f}, {30.0f, 16.0f}, {54.0f, 112.0f});

		builder.addRoom(hallRoom);
		builder.addRoom(bikeRoom);
		builder.addRoom(mainRoom);
		builder.addFrame(bikeRoomFrame);

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