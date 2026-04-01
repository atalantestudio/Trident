// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> VisibilityRenderer::createMaterialOffsetPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {materialOffsetDescriptorSetLayout},
			#if defined(BUILD_MODE_DEBUG)
				.name = "Material offset",
			#endif
		});
	}
}