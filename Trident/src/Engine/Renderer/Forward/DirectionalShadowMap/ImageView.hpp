// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
		ref<ImageView> ForwardRenderer::createDirectionalShadowMapImageView() const {
			return directionalShadowMapImage->createView({
				.type = ImageViewType::TYPE_2D,
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = 0,
				.layerCount = directionalShadowMapImage->getLayerCount(),
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Directional shadow map view",
				#endif
			});
		}

		ref<ImageView> ForwardRenderer::createDirectionalShadowMapCascadeImageView(uint32 cascadeIndex) const {
			#if defined(DEBUG_OBJECT_NAME)
				sequence<char8> name = Logger::format("Directional shadow map view (cascade [])", cascadeIndex);
			#endif

			return directionalShadowMapImage->createView({
				.type = ImageViewType::TYPE_2D,
				.mipLevelOffset = 0,
				.mipLevelCount = 1,
				.layerOffset = cascadeIndex,
				.layerCount = 1,
				#if defined(DEBUG_OBJECT_NAME)
					.name = name,
				#endif
			});
		}
	#endif
}