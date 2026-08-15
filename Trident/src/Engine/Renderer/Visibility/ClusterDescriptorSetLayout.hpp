// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createClusterDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Vertex position component
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX,
			},

			// Vertex position index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX,
			},

			// Cluster
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = (
					ShaderStage::VERTEX | ShaderStage::FRAGMENT
					#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
						| ShaderStage::COMPUTE
					#endif
				),
			},
		};

		return createDescriptorSetLayout(bindings);
	}
}