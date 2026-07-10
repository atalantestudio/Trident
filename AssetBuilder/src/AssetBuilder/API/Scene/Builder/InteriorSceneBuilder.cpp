// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"

namespace Trident {
	InteriorSceneBuilder::InteriorSceneBuilder(std::filesystem::path rootOutputPath, view<char8> assetName) :
		StaticSceneBuilder(rootOutputPath, assetName)
	{
		allocateMaterials(1);
		insertMaterial(0, {
			.albedo = {1.0f, 1.0f, 1.0f, 1.0f},
			.emissiveColor = {0.0f, 0.0f, 0.0f, 1.0f},
			.emissiveStrength = 0.0f,
		});
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

		const float32 floorLevel = floorLevels[baseFloorLevelIndex];
		const float32 ceilingLevel = floorLevels[baseFloorLevelIndex + 1];
		const float32 sizeY = ceilingLevel - floorLevel;
		const float32 halfSizeX = sizeX * 0.5f;
		const float32 halfSizeY = sizeY * 0.5f;
		const float32 halfSizeZ = sizeZ * 0.5f;

		// Floor (towards +Y)
		createStaticObject({
			.geometryAssetName = "UnitPlane",
			.transform = {
				.position = {0.0f, floorLevel, 0.0f},
				.rotation = {0.0f, 0.0f, 0.0f},
				.scale = {sizeX, 0.0f, sizeZ},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {1.0f, 1.0f},
			},
			.materialIndex = 0,
			.lightmapPixelScale = {4.0f, 4.0f},
		});

		// Ceiling (towards -Y)
		createStaticObject({
			.geometryAssetName = "UnitPlane",
			.transform = {
				.position = {0.0f, ceilingLevel, 0.0f},
				.rotation = {pi<float32>(), 0.0f, 0.0f},
				.scale = {sizeX, 0.0f, sizeZ},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {1.0f, 1.0f},
			},
			.materialIndex = 0,
			.lightmapPixelScale = {4.0f, 4.0f},
		});

		// Left wall (towards +X)
		createStaticObject({
			.geometryAssetName = "UnitPlane",
			.transform = {
				.position = {-halfSizeX, floorLevel + halfSizeY, 0.0f},
				.rotation = {0.0f, 0.0f, -pi<float32>() * 0.5f},
				.scale = {0.0f, sizeY, sizeZ},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {1.0f, 1.0f},
			},
			.materialIndex = 0,
			.lightmapPixelScale = {4.0f, 4.0f},
		});

		// Right wall (towards -X)
		createStaticObject({
			.geometryAssetName = "UnitPlane",
			.transform = {
				.position = {halfSizeX, floorLevel + halfSizeY, 0.0f},
				.rotation = {0.0f, 0.0f, pi<float32>() * 0.5f},
				.scale = {0.0f, sizeY, sizeZ},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {1.0f, 1.0f},
			},
			.materialIndex = 0,
			.lightmapPixelScale = {4.0f, 4.0f},
		});

		// Back wall (towards +Z)
		createStaticObject({
			.geometryAssetName = "UnitPlane",
			.transform = {
				.position = {0.0f, floorLevel + halfSizeY, -halfSizeZ},
				.rotation = {pi<float32>() * 0.5f, 0.0f, 0.0f},
				.scale = {sizeX, sizeY, 0.0f},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {1.0f, 1.0f},
			},
			.materialIndex = 0,
			.lightmapPixelScale = {4.0f, 4.0f},
		});

		// Front wall (towards -Z)
		createStaticObject({
			.geometryAssetName = "UnitPlane",
			.transform = {
				.position = {0.0f, floorLevel + halfSizeY, halfSizeZ},
				.rotation = {-pi<float32>() * 0.5f, 0.0f, 0.0f},
				.scale = {sizeX, sizeY, 0.0f},
			},
			.textureFiltering = {
				.translation = {0.0f, 0.0f},
				.rotation = 0.0f,
				.scale = {1.0f, 1.0f},
			},
			.materialIndex = 0,
			.lightmapPixelScale = {4.0f, 4.0f},
		});
	}
}