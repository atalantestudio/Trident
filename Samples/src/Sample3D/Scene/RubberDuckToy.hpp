// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	Scene3D createRubberDuckToyScene(AssetLibrary* assetLibrary, GraphicsAPI graphicsApi) {
		Scene3D scene;

		const ref<GeometryAsset> geometryAsset = assetLibrary->getAsset<GeometryAsset>("rubber_duck_toy_1k");
		const ref<Geometry3D> geometry = createRef<Geometry3D>(geometryAsset);
		const uint32 geometryIndex = scene.registerGeometry(geometry);

		DescriptorSetLayoutBinding armBinding{};
		armBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		armBinding.descriptorCount = 1;
		armBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding diffuseBinding{};
		diffuseBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		diffuseBinding.descriptorCount = 1;
		diffuseBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding normalBinding{};
		normalBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		normalBinding.descriptorCount = 1;
		normalBinding.shaderStages = ShaderStage::FRAGMENT;

		Material pbrMaterial{};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				pbrMaterial.shaderModulePath = "Material/PBR.frag";

				break;
			}
			case GraphicsAPI::GL: {
				pbrMaterial.shaderModulePath = "Material/PBRSingleSet.frag";

				break;
			}
		}

		pbrMaterial.descriptorSetLayoutBindings = {
			armBinding,
			diffuseBinding,
			normalBinding,
		};

		const uint32 pbrMaterialIndex = scene.registerMaterial(pbrMaterial);

		MaterialReference pbrMaterialReference{};
		pbrMaterialReference.materialIndex = pbrMaterialIndex;
		pbrMaterialReference.descriptorImageAssetPaths = {
			// TODO: Generate assets in the correct subdirectory.
			// "rubber_duck_toy_1k/textures/rubber_duck_toy_arm_1k",
			// "rubber_duck_toy_1k/textures/rubber_duck_toy_diff_1k",
			// "rubber_duck_toy_1k/textures/rubber_duck_toy_nor_gl_1k",
			"rubber_duck_toy_arm_1k",
			"rubber_duck_toy_diff_1k",
			"rubber_duck_toy_nor_gl_1k",
		};

		const uint32 pbrMaterialReferenceIndex = scene.registerMaterialReference(pbrMaterialReference);

		SceneInstanceDescriptor instanceDescriptor{};
		instanceDescriptor.position = {0.0_m, -0.125_m, 0.0_m};
		instanceDescriptor.rotation = {pi<float32>() * 0.5f, -pi<float32>() * 0.5f, 0.0_rad};
		instanceDescriptor.geometryIndex = geometryIndex;
		instanceDescriptor.materialReferenceIndex = pbrMaterialReferenceIndex;

		scene.registerInstance(instanceDescriptor);

		return scene;
	}
}