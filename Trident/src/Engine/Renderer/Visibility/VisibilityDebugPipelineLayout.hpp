// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
		ref<PipelineLayout> VisibilityRenderer::createVisibilityDebugPipelineLayout() const {
			return createPipelineLayout({
				.descriptorSetLayouts = {
					visibilityDebugDescriptorSetLayout,
					clusterDescriptorSetLayout,
					instanceDescriptorSetLayout,
				},
				#if defined(BUILD_MODE_DEBUG)
					.name = "Visibility debug",
				#endif
			});
		}
	#endif
}