// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<PipelineLayout> Lightmapper::createPostProcessingPipelineLayout() const {
		return device->createPipelineLayout({
			.descriptorSetLayouts = {descriptorSetLayouts[2]},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Post processing",
			#endif
		});
	}
}