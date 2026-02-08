// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	static Scene3D createScene(const AssetLibrary& assetLibrary) {
		Scene3D scene;

		const ref<GeometryAsset> geometryAsset = assetLibrary.findGeometryAsset("lemon_1k");
		const ref<Geometry3D> geometry = createRef<Geometry3D>(geometryAsset);
		const uint32 geometryIndex = scene.registerGeometry(geometry);

		DescriptorSetLayoutBinding aoBinding{};
		aoBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		aoBinding.descriptorCount = 1;
		aoBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding roughnessBinding{};
		roughnessBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		roughnessBinding.descriptorCount = 1;
		roughnessBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding diffuseBinding{};
		diffuseBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		diffuseBinding.descriptorCount = 1;
		diffuseBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding normalBinding{};
		normalBinding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
		normalBinding.descriptorCount = 1;
		normalBinding.shaderStages = ShaderStage::FRAGMENT;

		Material material{};
		material.shaderModulePath = "Material/PBRSeparatedAORoughness.frag";
		material.descriptorSetLayoutBindings = {
			aoBinding,
			roughnessBinding,
			diffuseBinding,
			normalBinding,
		};

		const uint32 materialIndex = scene.registerMaterial(material);

		MaterialReference materialReference{};
		materialReference.materialIndex = materialIndex;
		materialReference.descriptorImageAssetPaths = {
			"lemon_ao_1k",
			"lemon_rough_1k",
			"lemon_diff_1k",
			"lemon_nor_gl_1k",
		};

		const uint32 materialReferenceIndex = scene.registerMaterialReference(materialReference);

		InstanceDescriptor instance1Descriptor{};
		instance1Descriptor.position = {0.3_m, 0.0_m, 1.0_m};
		instance1Descriptor.rotation = {-pi<float32>() / 2.0f, -0.2_rad, pi<float32>()};
		instance1Descriptor.scale = {10.0f, 10.0f, 10.0f};
		instance1Descriptor.geometryIndex = geometryIndex;
		instance1Descriptor.materialReferenceIndex = materialReferenceIndex;

		scene.registerInstance(instance1Descriptor);

		InstanceDescriptor instance2Descriptor{};
		instance2Descriptor.position = {-0.35_m, -0.2_m, 1.0_m};
		instance2Descriptor.rotation = {-0.3_rad, -0.3_rad, pi<float32>() / 2.0f};
		instance2Descriptor.scale = {10.0f, 10.0f, 10.0f};
		instance2Descriptor.geometryIndex = geometryIndex;
		instance2Descriptor.materialReferenceIndex = materialReferenceIndex;

		scene.registerInstance(instance2Descriptor);

		return scene;
	}
}