#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createMaterialCoordinateDescriptorSetLayout() const {
		return createDescriptorSetLayout({
			// View
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

			// Material offset
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},

			// Material coordinate
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::COMPUTE,
			},
		});
	}
}