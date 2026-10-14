// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
		ref<DescriptorSetLayout> ForwardRenderer::createGIVertexGenerationDescriptorSetLayout() const {
			const DescriptorSetLayoutBinding bindings[] = {
				// Camera
				{
					.descriptorType = DescriptorType::UNIFORM_BUFFER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::VERTEX,
				},

				// Vertex
				{
					.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::COMPUTE,
				},

				// Index
				{
					.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::COMPUTE,
				},

				// Point light
				{
					.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::COMPUTE,
				},

				// GI vertex
				{
					.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::COMPUTE,
				},
			};

			return createDescriptorSetLayout({
				.bindings = bindings,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "GI vertex generation",
				#endif
			});
		}
	#endif
}