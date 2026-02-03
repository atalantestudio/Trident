// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createOutputDescriptorSetLayout() const {
		DescriptorSetLayoutBinding outputBinding{};
		outputBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE;
		outputBinding.descriptorCount = 1;
		outputBinding.shaderStages = ShaderStage::FRAGMENT;

		return createDescriptorSetLayout({outputBinding});
	}
}