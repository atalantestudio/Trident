// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Debug/Renderer.hpp"

namespace Trident {
	ref<PipelineLayout> DebugRenderer::createSkyboxPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {skyboxDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Skybox",
			#endif
		});
	}
}