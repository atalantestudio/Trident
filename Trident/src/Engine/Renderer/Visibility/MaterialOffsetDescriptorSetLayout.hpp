// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createMaterialOffsetDescriptorSetLayout() const {
		DescriptorSetLayoutBinding materialCountBufferBinding{};
		materialCountBufferBinding.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER;
		materialCountBufferBinding.descriptorCount = 1;
		materialCountBufferBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding materialOffsetBufferBinding{};
		materialOffsetBufferBinding.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER;
		materialOffsetBufferBinding.descriptorCount = 1;
		materialOffsetBufferBinding.shaderStages = ShaderStage::COMPUTE;

		DescriptorSetLayoutBinding materialIndirectBufferBinding{};
		materialIndirectBufferBinding.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER;
		materialIndirectBufferBinding.descriptorCount = 1;
		materialIndirectBufferBinding.shaderStages = ShaderStage::COMPUTE;

		return createDescriptorSetLayout({
			materialCountBufferBinding,
			materialOffsetBufferBinding,
			materialIndirectBufferBinding,
		});
	}
}