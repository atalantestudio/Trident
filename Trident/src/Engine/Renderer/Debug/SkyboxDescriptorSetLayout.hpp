// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Debug/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> DebugRenderer::createSkyboxDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Skybox camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::VERTEX,
			},

			// Skybox
			{
				.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::FRAGMENT,
			},
		};

		return createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Skybox",
			#endif
		});
	}
}