// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	inline MaterialDescriptorImage getLightmapAtlas(AssetLibrary* assetLibrary) {
		return {
			.imageAsset = assetLibrary->getAsset<ImageAsset>("Hall"),
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
		};
	}

	inline void setLightmappedShaderModule(MaterialAsset& asset, AssetLibrary* assetLibrary) {
		asset.shaderModuleName = "Lightmapped";
		asset.stage = ShaderStage::FRAGMENT;
		asset.entryPoint = "main";
		asset.descriptorBindingTypes = {
			// Diffuse
			DescriptorType::COMBINED_IMAGE_SAMPLER,

			// Lightmap
			DescriptorType::COMBINED_IMAGE_SAMPLER,
		};
	}

	inline MaterialAsset createLightmappedFloorMaterial(AssetLibrary* assetLibrary) {
		MaterialAsset asset{};
		asset.identifier = 100;
		asset.descriptorImages = {
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("white"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			getLightmapAtlas(assetLibrary),
		};

		setLightmappedShaderModule(asset, assetLibrary);

		return asset;
	}

	inline MaterialAsset createLightmappedWallMaterial(AssetLibrary* assetLibrary) {
		MaterialAsset asset{};
		asset.identifier = 101;
		asset.descriptorImages = {
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("normal"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			getLightmapAtlas(assetLibrary),
		};

		setLightmappedShaderModule(asset, assetLibrary);

		return asset;
	}

	inline MaterialAsset createLightmappedDoorMaterial(AssetLibrary* assetLibrary) {
		MaterialAsset asset{};
		asset.identifier = 102;
		asset.descriptorImages = {
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("blue"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			getLightmapAtlas(assetLibrary),
		};

		setLightmappedShaderModule(asset, assetLibrary);

		return asset;
	}

	inline MaterialAsset createLightmappedTopWallMaterial(AssetLibrary* assetLibrary) {
		MaterialAsset asset{};
		asset.identifier = 103;
		asset.descriptorImages = {
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("green"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			getLightmapAtlas(assetLibrary),
		};

		setLightmappedShaderModule(asset, assetLibrary);

		return asset;
	}

	inline MaterialAsset createLightmappedCeilingMaterial(AssetLibrary* assetLibrary) {
		MaterialAsset asset{};
		asset.identifier = 104;
		asset.descriptorImages = {
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("red"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			getLightmapAtlas(assetLibrary),
		};

		setLightmappedShaderModule(asset, assetLibrary);

		return asset;
	}
}