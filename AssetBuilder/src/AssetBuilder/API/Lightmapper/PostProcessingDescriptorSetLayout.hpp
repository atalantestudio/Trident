// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<DescriptorSetLayout> Lightmapper::createPostProcessingDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[1] = {
			// Intermediate lightmap atlas
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},
		};

		return Renderer::createDescriptorSetLayout(view<DescriptorSetLayoutBinding>(bindings, 1));
	}
}