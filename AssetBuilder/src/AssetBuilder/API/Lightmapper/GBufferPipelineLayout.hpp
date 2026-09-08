// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<PipelineLayout> Lightmapper::createGBufferPipelineLayout() const {
		return device->createPipelineLayout({
			.descriptorSetLayouts = {descriptorSetLayouts[0]},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer",
			#endif
		});
	}
}