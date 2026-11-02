// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> ForwardRenderer::createHullWireframePipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {sceneDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Hull wireframe",
			#endif
		});
	}
}