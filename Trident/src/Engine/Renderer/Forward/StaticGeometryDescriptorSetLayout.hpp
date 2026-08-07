// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> ForwardRenderer::createStaticGeometryDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[2] = {
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX | ShaderStage::FRAGMENT,
			},

			// Lightmap atlas
			{
				.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},
		};

		return createDescriptorSetLayout(view<DescriptorSetLayoutBinding>(bindings, 2));
	}
}