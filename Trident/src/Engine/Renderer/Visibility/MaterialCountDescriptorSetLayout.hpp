// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createMaterialCountDescriptorSetLayout() const {
		return createDescriptorSetLayout({
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Visibility
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Cluster
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Instance
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Material count
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},
		});
	}
}