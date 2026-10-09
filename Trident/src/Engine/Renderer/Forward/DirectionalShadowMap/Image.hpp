// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<Image> ForwardRenderer::createDirectionalShadowMapImage() const {
		return directionalShadowMapRenderPass->createDepthStencilAttachmentImage(
			{
				.type = ImageType::TYPE_2D,
				.extent = {DIRECTIONAL_SHADOW_MAP_EXTENT.x, DIRECTIONAL_SHADOW_MAP_EXTENT.y, 1},
				.mipLevelCount = 1,
				.layerCount = DIRECTIONAL_SHADOW_MAP_CASCADE_COUNT,
				.additionalUsage = ImageUsage::SAMPLED,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Directional shadow map",
				#endif
			},
			0
		);
	}
}