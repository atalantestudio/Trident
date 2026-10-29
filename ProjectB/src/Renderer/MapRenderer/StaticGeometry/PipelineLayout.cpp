// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	ref<PipelineLayout> MapRenderer::createStaticGeometryPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {staticGeometryDescriptorSetLayout},
			#if defined(BUILD_MODE_DEBUG)
				.name = "Static geometry",
			#endif
		});
	}
}