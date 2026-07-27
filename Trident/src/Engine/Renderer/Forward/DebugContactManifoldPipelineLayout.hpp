// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> ForwardRenderer::createDebugContactManifoldPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {forwardDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Debug contact manifold",
			#endif
		});
	}
}