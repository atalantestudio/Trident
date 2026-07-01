// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Material/Shader.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident::LemonAndStool {
	Scene3D initializeScene(Application* application, GraphicsAPI graphicsApi, uint64 minStorageBufferOffsetAlignment) {
		AssetLibrary* assetLibrary = application->getAssetLibrary();

		assetLibrary->getAsset<GeometryAsset>("lemon_1k");
		assetLibrary->getAsset<GeometryAsset>("folding_wooden_stool");

		const view<char8> sceneAssetName = application->getPropertyFile()->getProperty<view<char8>>("AssetName");
		const ref<Scene3DAsset> sceneAsset = assetLibrary->getAsset<Scene3DAsset>(sceneAssetName);

		Scene3D scene(sceneAsset, minStorageBufferOffsetAlignment);

		Shader defaultMaterialShader = {
			.descriptorSetLayoutBindings = {},
		};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				defaultMaterialShader.modulePath = "Material/Default";

				break;
			}
			case GraphicsAPI::GL: {
				defaultMaterialShader.modulePath = "Material/DefaultGL";

				break;
			}
		}

		const uint32 defaultMaterialShaderIndex = scene.registerMaterialShader(defaultMaterialShader);

		scene.registerMaterial({
			.shaderIndex = defaultMaterialShaderIndex,
			.writeDescriptorSetBindings = {},
		});

		Shader pbrMaterialShader = {
			.descriptorSetLayoutBindings = {
				// Ambient occlusion, roughness, metallic
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},

				// Diffuse
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},

				// Normal
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},
			},
		};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				pbrMaterialShader.modulePath = "Material/Test";

				break;
			}
			case GraphicsAPI::GL: {
				pbrMaterialShader.modulePath = "Material/TestGL";

				break;
			}
		}

		const uint32 pbrMaterialShaderIndex = scene.registerMaterialShader(pbrMaterialShader);

		scene.registerMaterial({
			.shaderIndex = pbrMaterialShaderIndex,
			.writeDescriptorSetBindings = {
				{
					.descriptorImages = {
						{
							// TODO: lemon_1k must use PBRSeparatedAORoughness.
							.assetPath = "lemon_ao_1k",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "lemon_diff_1k",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "lemon_nor_gl_1k",
						},
					},
				},
			},
		});

		scene.registerMaterial({
			.shaderIndex = pbrMaterialShaderIndex,
			.writeDescriptorSetBindings = {
				{
					.descriptorImages = {
						{
							.assetPath = "folding_wooden_stool_arm_1k",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "folding_wooden_stool_diff_1k",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "folding_wooden_stool_nor_gl_1k",
						},
					},
				},
			},
		});

		return scene;
	}
}