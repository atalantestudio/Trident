// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Experimental/GI2D/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> GI2DExperimentalRenderer::createGlobalIlluminationPipelineLayout() const {
		return Renderer::createPipelineLayout({
			.descriptorSetLayouts = {descriptorSetLayouts[1]},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Global illumination",
			#endif
		});
	}
}