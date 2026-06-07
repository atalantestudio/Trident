// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Scene/StaticSceneRayTracer/StaticSceneRayTracer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> StaticSceneRayTracer::createPresentDescriptorSetLayout() const {
		return createDescriptorSetLayout({
			// Image
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.shaderStages = ShaderStage::FRAGMENT,
			},
		});
	}
}