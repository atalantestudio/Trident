// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> ForwardRenderer::createForwardDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[2] = {
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX | ShaderStage::FRAGMENT,
			},

			// Point light
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},
		};

		return createDescriptorSetLayout(view<DescriptorSetLayoutBinding>(bindings, 2));
	}
}