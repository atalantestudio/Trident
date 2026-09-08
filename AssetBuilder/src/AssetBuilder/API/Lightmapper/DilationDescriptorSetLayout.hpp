// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<DescriptorSetLayout> Lightmapper::createDilationDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Intermediate lightmap atlas
			{
				.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::FRAGMENT,
			},
		};

		return device->createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Dilation",
			#endif
		});
	}
}