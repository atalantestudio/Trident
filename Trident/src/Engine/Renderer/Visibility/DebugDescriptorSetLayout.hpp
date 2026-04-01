#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createDebugDescriptorSetLayout() const {
		return createDescriptorSetLayout({
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},

			// Visibility
			{
				.descriptorType = DescriptorType::INPUT_ATTACHMENT,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},

			// Depth stencil
			{
				.descriptorType = DescriptorType::INPUT_ATTACHMENT,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},

			// Vertex position component
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},

			// Vertex normal component
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},

			// Vertex texture coordinate
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},

			// Vertex position index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},

			// Vertex normal index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},

			// Vertex texture coordinate index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},
		});
	}
}