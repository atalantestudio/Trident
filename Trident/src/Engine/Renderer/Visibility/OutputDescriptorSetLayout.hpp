// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createOutputDescriptorSetLayout() const {
		return createDescriptorSetLayout({
			// Output
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},
		});
	}
}