// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Debug/Renderer.hpp"

namespace Trident {
	ref<ImageView> DebugRenderer::createSkyboxImageView() const {
		return skyboxImage->createView({
			.type = ImageViewType::TYPE_CUBE,
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 6,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Skybox view",
			#endif
		});
	}
}