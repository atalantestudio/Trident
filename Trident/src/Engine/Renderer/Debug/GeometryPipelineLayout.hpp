// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Debug/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> DebugRenderer::createGeometryPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {geometryDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Geometry",
			#endif
		});
	}
}