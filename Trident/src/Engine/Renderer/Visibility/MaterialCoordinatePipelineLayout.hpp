// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> VisibilityRenderer::createMaterialCoordinatePipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {materialCoordinateDescriptorSetLayout},
			#if defined(BUILD_MODE_DEBUG)
				.name = "Material coordinate",
			#endif
		});
	}
}