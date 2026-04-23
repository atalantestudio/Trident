// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> VisibilityRenderer::createMaterialCountPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {materialCountDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Material count",
			#endif
		});
	}
}