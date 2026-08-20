// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createMaterialOffsetDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Material count
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Material offset
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Material indirect
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},
		};

		return createDescriptorSetLayout(bindings);
	}
}