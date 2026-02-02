// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createVisibilityMaterialCountDescriptorSetLayout() const {
		DescriptorSetLayoutBinding materialCountBufferBinding{};
		materialCountBufferBinding.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER;
		materialCountBufferBinding.descriptorCount = 1;
		materialCountBufferBinding.shaderStages = ShaderStage::FRAGMENT;

		return createDescriptorSetLayout({materialCountBufferBinding});
	}
}