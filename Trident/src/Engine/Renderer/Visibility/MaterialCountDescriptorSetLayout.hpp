#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createMaterialCountDescriptorSetLayout() const {
		DescriptorSetLayoutBinding viewBinding{};
		viewBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		viewBinding.descriptorCount = 1;
		viewBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding visibilityBinding{};
		visibilityBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE;
		visibilityBinding.descriptorCount = 1;
		visibilityBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding clusterBinding{};
		clusterBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		clusterBinding.descriptorCount = 1;
		clusterBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding instanceBinding{};
		instanceBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		instanceBinding.descriptorCount = 1;
		instanceBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding materialCountBinding{};
		materialCountBinding.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER;
		materialCountBinding.descriptorCount = 1;
		materialCountBinding.shaderStages = ShaderStage::COMPUTE;

		return createDescriptorSetLayout({
			viewBinding,
			visibilityBinding,
			clusterBinding,
			instanceBinding,
			materialCountBinding,
		});
	}
}