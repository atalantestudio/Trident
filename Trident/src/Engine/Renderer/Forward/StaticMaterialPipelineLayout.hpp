// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> ForwardRenderer::createStaticMaterialPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {cameraDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Static material",
			#endif
		});
	}
}