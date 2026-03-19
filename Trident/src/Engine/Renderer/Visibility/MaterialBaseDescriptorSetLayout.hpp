// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createMaterialBaseDescriptorSetLayout() const {
		DescriptorSetLayoutBinding viewBinding{};
		viewBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		viewBinding.descriptorCount = 1;
		viewBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding materialOffsetBinding{};
		materialOffsetBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		materialOffsetBinding.descriptorCount = 1;
		materialOffsetBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding materialCoordinateBinding{};
		materialCoordinateBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		materialCoordinateBinding.descriptorCount = 1;
		materialCoordinateBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding outputBinding{};
		outputBinding.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE;
		outputBinding.descriptorCount = 1;
		outputBinding.shaderStages = ShaderStage::COMPUTE;

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

		return createDescriptorSetLayout({
			viewBinding,
			materialOffsetBinding,
			materialCoordinateBinding,
			outputBinding,
			visibilityBinding,
			clusterBinding,
			instanceBinding,
		});
	}
}