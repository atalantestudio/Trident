// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<PipelineLayout> Lightmapper::createRayTracingPipelineLayout() const {
		return device->createPipelineLayout({
			.descriptorSetLayouts = {descriptorSetLayouts[1]},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Ray tracing",
			#endif
		});
	}
}