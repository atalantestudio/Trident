// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
		ref<DescriptorSetLayout> ForwardRenderer::createDirectionalShadowMapDescriptorSetLayout() const {
			const DescriptorSetLayoutBinding bindings[] = {
				// Directional light
				{
					.descriptorType = DescriptorType::UNIFORM_BUFFER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::VERTEX | ShaderStage::FRAGMENT,
				},
			};

			return createDescriptorSetLayout({
				.bindings = bindings,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Directional shadow map",
				#endif
			});
		}
	#endif
}