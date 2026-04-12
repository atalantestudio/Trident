// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident::Box {
	Scene3D initializeScene(Application* application, GraphicsAPI graphicsApi, uint64 minStorageBufferOffsetAlignment) {
		AssetLibrary* assetLibrary = application->getAssetLibrary();

		assetLibrary->getAsset<GeometryAsset>("UnitBox");

		const view<char8> sceneAssetName = application->getPropertyFile()->getProperty<view<char8>>("AssetName");
		const ref<Scene3DAsset> sceneAsset = assetLibrary->getAsset<Scene3DAsset>(sceneAssetName);

		Scene3D scene(sceneAsset, minStorageBufferOffsetAlignment);

		Shader pbrMaterialShader = {
			.descriptorSetLayoutBindings = {
				// Ambient occlusion
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				},

				// Roughness
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				},

				// Diffuse
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				},

				// Normal
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				},
			},
		};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				pbrMaterialShader.modulePath = "Material/PBRSeparatedAORoughness.frag";

				break;
			}
			case GraphicsAPI::GL: {
				pbrMaterialShader.modulePath = "Material/PBRSeparatedAORoughnessSingleSet.frag";

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
							.assetPath = "checkerboard1",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "checkerboard1",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "checkerboard1",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "checkerboard1",
						},
					},
				},
			},
		});

		return scene;
	}
}