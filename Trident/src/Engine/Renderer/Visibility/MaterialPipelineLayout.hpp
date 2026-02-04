// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	void VisibilityRenderer::createMaterialPipelineLayouts() {
		materialPipelineLayouts = sequence<ref<PipelineLayout>>(scene->materials.count());

		PipelineLayoutDescriptor materialPipelineLayoutDescriptor{};
		materialPipelineLayoutDescriptor.descriptorSetLayouts = {materialBaseDescriptorSetLayout};

		#if defined(BUILD_MODE_DEBUG)
			sequence<char8> name;
		#endif

		uint32 materialPipelineLayoutIndex = 0;

		while (materialPipelineLayoutIndex < materialPipelineLayouts.count()) {
			#if defined(BUILD_MODE_DEBUG)
				name = Logger::format("Material []", materialPipelineLayoutIndex);

				materialPipelineLayoutDescriptor.name = name;
			#endif

			materialPipelineLayouts[materialPipelineLayoutIndex] = createPipelineLayout(materialPipelineLayoutDescriptor);

			materialPipelineLayoutIndex += 1;
		}
	}
}