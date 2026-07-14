// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"

namespace Trident {
	InteriorSceneBuilder::InteriorSceneBuilder(std::filesystem::path rootOutputPath, view<char8> assetName) :
		StaticSceneBuilder(rootOutputPath, assetName)
	{
		addMaterialShader(
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

		addMaterial(100, 0);
	}

	uint32 InteriorSceneBuilder::addFloorLevel(float32 level) {
		const uint32 index = static_cast<uint32>(floorLevels.size());

		if (index > 0) {
			DEBUG_ASSERT(level > floorLevels[index - 1]);
		}

		floorLevels.emplace_back(level);

		return index;
	}

	void InteriorSceneBuilder::addRoom(uint32 baseFloorLevelIndex, float32 sizeX, float32 sizeZ) {
		DEBUG_ASSERT(floorLevels.size() > baseFloorLevelIndex + 1);

		// Texture scale multiplier
		constexpr float32 m1 = 1.0f / 256.0f;

		const float32 floorLevel = floorLevels[baseFloorLevelIndex];
		const float32 ceilingLevel = floorLevels[baseFloorLevelIndex + 1];
		const float32 sizeY = ceilingLevel - floorLevel;
		const float32 halfSizeX = sizeX * 0.5f;
		const float32 halfSizeY = sizeY * 0.5f;
		const float32 halfSizeZ = sizeZ * 0.5f;

		// Floor (towards +Y)
		addSurface({
			.materialIndex = 0,
			.transform = {
				.position = {0.0f, floorLevel, 0.0f},
				.rotation = {0.0f, 0.0f, 0.0f},
				.scale = {sizeX, sizeZ},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {m1 / 0.25f, m1 / 0.25f},
			},
			.lightmapScale = {4.0f, 4.0f},
		});

		// Ceiling (towards -Y)
		addSurface({
			.materialIndex = 0,
			.transform = {
				.position = {0.0f, ceilingLevel, 0.0f},
				.rotation = {0.0f, 0.0f, pi<float32>()},
				.scale = {sizeX, sizeZ},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {m1 / 0.25f, m1 / 0.25f},
			},
			.lightmapScale = {4.0f, 4.0f},
		});

		// Left wall (towards +X)
		addSurface({
			.materialIndex = 0,
			.transform = {
				.position = {-halfSizeX, floorLevel + halfSizeY, 0.0f},
				.rotation = {0.0f, pi<float32>() * 0.5f, pi<float32>() * 0.5f},
				.scale = {sizeZ, sizeY},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {m1 / 0.25f, m1 / 0.25f},
			},
			.lightmapScale = {4.0f, 4.0f},
		});

		// Right wall (towards -X)
		addSurface({
			.materialIndex = 0,
			.transform = {
				.position = {halfSizeX, floorLevel + halfSizeY, 0.0f},
				.rotation = {0.0f, -pi<float32>() * 0.5f, -pi<float32>() * 0.5f},
				.scale = {sizeZ, sizeY},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {m1 / 0.25f, m1 / 0.25f},
			},
			.lightmapScale = {4.0f, 4.0f},
		});

		// Back wall (towards +Z)
		addSurface({
			.materialIndex = 0,
			.transform = {
				.position = {0.0f, floorLevel + halfSizeY, -halfSizeZ},
				.rotation = {-pi<float32>() * 0.5f, 0.0f, pi<float32>()},
				.scale = {sizeX, sizeY},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {m1 / 0.25f, m1 / 0.25f},
			},
			.lightmapScale = {4.0f, 4.0f},
		});

		// Front wall (towards -Z)
		addSurface({
			.materialIndex = 0,
			.transform = {
				.position = {0.0f, floorLevel + halfSizeY, halfSizeZ},
				.rotation = {pi<float32>() * 0.5f, 0.0f, 0.0f},
				.scale = {sizeX, sizeY},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {m1 / 0.25f, m1 / 0.25f},
			},
			.lightmapScale = {4.0f, 4.0f},
		});
	}
}