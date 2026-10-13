// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
		ref<PipelineLayout> ForwardRenderer::createDirectionalShadowMapPipelineLayout() const {
			return createPipelineLayout({
				.descriptorSetLayouts = {directionalShadowMapDescriptorSetLayout},
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Directional shadow map",
				#endif
			});
		}
	#endif
}