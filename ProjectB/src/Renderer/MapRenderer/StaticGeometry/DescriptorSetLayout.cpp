// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	ref<DescriptorSetLayout> MapRenderer::createStaticGeometryDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::VERTEX | ShaderStage::FRAGMENT,
			},
		};

		return createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Static geometry",
			#endif
		});
	}
}