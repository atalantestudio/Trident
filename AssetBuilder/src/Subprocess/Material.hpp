// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// TODO(filesystem/asset/material): Material assets shouldn't reference a scene's lightmap.
	inline MaterialDescriptorImage getLightmapAtlas(AssetLibrary* assetLibrary) {
		return {
			.imageAsset = assetLibrary->getAsset<ImageAsset>("Hall"),
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
		};
	}

	inline MaterialAsset createLightmappedFloorMaterial(AssetLibrary* assetLibrary) {
		MaterialAsset asset{};
		asset.identifier = 100;
		asset.descriptorImages = {
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("tilefloor018a"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			getLightmapAtlas(assetLibrary),
		};

		return asset;
	}

	inline MaterialAsset createLightmappedWallMaterial(AssetLibrary* assetLibrary) {
		MaterialAsset asset{};
		asset.identifier = 101;
		asset.descriptorImages = {
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("plasterwall030c"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			getLightmapAtlas(assetLibrary),
		};

		return asset;
	}

	inline MaterialAsset createLightmappedDoorMaterial(AssetLibrary* assetLibrary) {
		MaterialAsset asset{};
		asset.identifier = 102;
		asset.descriptorImages = {
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("wooddoor048a"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			getLightmapAtlas(assetLibrary),
		};

		return asset;
	}

	inline MaterialAsset createLightmappedCeilingMaterial(AssetLibrary* assetLibrary) {
		MaterialAsset asset{};
		asset.identifier = 103;
		asset.descriptorImages = {
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("plasterwall030a"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			getLightmapAtlas(assetLibrary),
		};

		return asset;
	}
}