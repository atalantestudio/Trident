// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/Geometry3D.hpp"
#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Material/Shader.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/ECS/Component/MaterialComponent.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident::Lemons {
	Scene3D initializeScene(Application* application, GraphicsAPI graphicsApi) {
		AssetLibrary* assetLibrary = application->getAssetLibrary();

		const ref<GeometryAsset> lemonGeometryAsset = assetLibrary->getAsset<GeometryAsset>("lemon_1k");

		const view<char8> sceneAssetName = application->getPropertyFile()->getProperty<view<char8>>("AssetName");
		const ref<Scene3DAsset> sceneAsset = assetLibrary->getAsset<Scene3DAsset>(sceneAssetName);

		Scene3D scene(sceneAsset);

		/* Shader defaultMaterialShader = {
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
		}); */

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
							.assetPath = "lemon_ao_1k",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "lemon_rough_1k",
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

		// TODO: Simplify.
		scene.ecs.getComponentPool<MaterialComponent>().setComponent(0, {0});
		scene.ecs.getComponentPool<MaterialComponent>().setComponent(1, {0});

		return scene;
	}
}