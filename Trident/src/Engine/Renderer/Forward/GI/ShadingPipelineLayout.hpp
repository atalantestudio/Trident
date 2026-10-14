// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
		ref<PipelineLayout> ForwardRenderer::createGIShadingPipelineLayout() const {
			return createPipelineLayout({
				.descriptorSetLayouts = {giVertexGenerationDescriptorSetLayout},
				#if defined(DEBUG_OBJECT_NAME)
					.name = "GI shading",
				#endif
			});
		}
	#endif
}