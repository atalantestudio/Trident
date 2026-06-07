// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Scene/StaticSceneRayTracer/StaticSceneRayTracer.hpp"

namespace Trident {
	ref<PipelineLayout> StaticSceneRayTracer::createRayTracingPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {rayTracingDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Ray tracing",
			#endif
		});
	}
}