// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> ForwardRenderer::createSceneDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::VERTEX | ShaderStage::FRAGMENT,
			},

			// Point light
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::FRAGMENT,
			},

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				// Directional light
				{
					.descriptorType = DescriptorType::UNIFORM_BUFFER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::FRAGMENT,
				},

				// Directional shadow map
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::FRAGMENT,
				},
			#endif
		};

		return createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Scene",
			#endif
		});
	}
}