// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> ForwardRenderer::createStaticDescriptorSetLayout() const {
		return createDescriptorSetLayout({
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX,
			},

			// Position
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX,
			},

			// Normal
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX,
			},

			// Texture coordinate
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::VERTEX,
			},
		});
	}
}