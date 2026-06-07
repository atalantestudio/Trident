// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Scene/StaticSceneRayTracer/StaticSceneRayTracer.hpp"

namespace Trident {
	ref<PipelineLayout> StaticSceneRayTracer::createPresentPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {presentDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Present",
			#endif
		});
	}
}