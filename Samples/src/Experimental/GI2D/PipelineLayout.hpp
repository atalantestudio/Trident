// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Experimental/GI2D/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> GI2DExperimentalRenderer::createPipelineLayout() const {
		return Renderer::createPipelineLayout({
			.descriptorSetLayouts = {},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Pipeline layout",
			#endif
		});
	}
}