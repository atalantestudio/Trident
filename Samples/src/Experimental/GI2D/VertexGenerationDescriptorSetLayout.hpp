// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Experimental/GI2D/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> GI2DExperimentalRenderer::createVertexGenerationDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Vertex
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},
		};

		return createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Vertex generation",
			#endif
		});
	}
}