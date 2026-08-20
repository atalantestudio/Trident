// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createInstanceDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Instance
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = (
					ShaderStage::VERTEX | ShaderStage::FRAGMENT
					#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
						| ShaderStage::COMPUTE
					#endif
				),
			},

			// Geometry
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = (
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