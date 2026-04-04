#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
		ref<DescriptorSetLayout> VisibilityRenderer::createVisibilityDebugDescriptorSetLayout() const {
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

				// Depth stencil
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},

				// Vertex position component
				{
					.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},

				// Vertex normal component
				{
					.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},

				// Vertex texture coordinate
				{
					.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},

				// Vertex position index
				{
					.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},

				// Vertex normal index
				{
					.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},

				// Vertex texture coordinate index
				{
					.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},

				// Output image
				{
					.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},
			});
		}
	#endif
}