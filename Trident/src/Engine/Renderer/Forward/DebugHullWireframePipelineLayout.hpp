// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> ForwardRenderer::createDebugHullWireframePipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {dynamicDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Debug hull wireframe",
			#endif
		});
	}
}