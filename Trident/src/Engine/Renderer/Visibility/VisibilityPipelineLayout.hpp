// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> VisibilityRenderer::createVisibilityPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {
				cameraDescriptorSetLayout,
				clusterDescriptorSetLayout,
				instanceDescriptorSetLayout,
			},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Visibility",
			#endif
		});
	}
}