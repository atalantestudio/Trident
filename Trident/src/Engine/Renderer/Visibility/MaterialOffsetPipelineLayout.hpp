// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createMaterialOffsetDescriptorSetLayout() const {
		DescriptorSetLayoutBinding materialCountBufferBinding{};
		materialCountBufferBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		materialCountBufferBinding.descriptorCount = 1;
		materialCountBufferBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding materialOffsetBufferBinding{};
		materialOffsetBufferBinding.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER;
		materialOffsetBufferBinding.descriptorCount = 1;
		materialOffsetBufferBinding.shaderStages = ShaderStage::COMPUTE;

		return createDescriptorSetLayout({
			materialCountBufferBinding,
			materialOffsetBufferBinding,
		});
	}

	ref<PipelineLayout> VisibilityRenderer::createMaterialOffsetPipelineLayout() const {
		PipelineLayoutDescriptor materialOffsetPipelineLayoutDescriptor{};
		materialOffsetPipelineLayoutDescriptor.descriptorSetLayouts = {materialOffsetDescriptorSetLayout};

		SET_DEBUG_NAME(materialOffsetPipelineLayoutDescriptor, "Material offset");

		return createPipelineLayout(materialOffsetPipelineLayoutDescriptor);
	}
}