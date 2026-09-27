// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident::LemonAndStool {
	ref<Scene3D> initializeScene(Application* application, GraphicsAPI graphicsApi) {
		const view<char8> sceneAssetName = application->getPropertyFile()->getProperty<view<char8>>("AssetName");
		const ref<Scene3DAsset> sceneAsset = application->getAssetLibrary()->getAsset<Scene3DAsset>(sceneAssetName);

		ref<Scene3D> scene = allocateRef<Scene3D>(sceneAsset);

		// TODO: Restore materials in this scene.
		/* const uint32 defaultMaterialShaderIndex = scene.registerMaterialShader();
		const uint32 pbrMaterialShaderIndex = scene.registerMaterialShader();

		MaterialAsset defaultMaterialAsset = {
			.stage = ShaderStage::COMPUTE,
			.entryPoint = "main",
			.descriptorBindingTypes = {},
			.descriptorImages = {},
		};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				defaultMaterialAsset.shaderModuleName = "Default";

				break;
			}
			case GraphicsAPI::GL: {
				defaultMaterialAsset.shaderModuleName = "DefaultGL";

				break;
			}
		}

		scene.registerMaterialAsset(allocateRef<MaterialAsset>(defaultMaterialAsset), defaultMaterialShaderIndex);

		MaterialAsset pbrMaterialAsset = {
			.stage = ShaderStage::COMPUTE,
			.entryPoint = "main",
			.descriptorBindingTypes = {
				// Ambient occlusion, roughness, metallic
				DescriptorType::COMBINED_IMAGE_SAMPLER,

				// Diffuse
				DescriptorType::COMBINED_IMAGE_SAMPLER,

				// Normal
				DescriptorType::COMBINED_IMAGE_SAMPLER,
			},
			.descriptorImages = {},
		};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				pbrMaterialAsset.shaderModuleName = "Test";

				break;
			}
			case GraphicsAPI::GL: {
				pbrMaterialAsset.shaderModuleName = "TestGL";

				break;
			}
		}

		MaterialAsset lemonMaterialAsset = pbrMaterialAsset;
		lemonMaterialAsset.descriptorImages = {
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("lemon_ao_1k"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("lemon_diff_1k"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("lemon_nor_gl_1k"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
		};

		scene.registerMaterialAsset(allocateRef<MaterialAsset>(lemonMaterialAsset), pbrMaterialShaderIndex);

		MaterialAsset stoolMaterialAsset = pbrMaterialAsset;
		stoolMaterialAsset.descriptorImages = {
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("folding_wooden_stool_arm_1k"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("folding_wooden_stool_diff_1k"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
			{
				.imageAsset = assetLibrary->getAsset<ImageAsset>("folding_wooden_stool_nor_gl_1k"),
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = 1,
			},
		};

		scene.registerMaterialAsset(allocateRef<MaterialAsset>(stoolMaterialAsset), pbrMaterialShaderIndex); */

		return scene;
	}
}