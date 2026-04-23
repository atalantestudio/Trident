// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> VisibilityRenderer::createMaterialCoordinatePipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {materialCoordinateDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Material coordinate",
			#endif
		});
	}
}