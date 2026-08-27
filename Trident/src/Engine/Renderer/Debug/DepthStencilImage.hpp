// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Debug/Renderer.hpp"

namespace Trident {
	ref<Image> DebugRenderer::createDepthStencilImage() const {
		const Extent2D extent = getSwapchain()->getExtent();

		return renderPass->createDepthStencilAttachmentImage(
			{
				.type = ImageType::TYPE_2D,
				.extent = {extent.width, extent.height, 1},
				.mipLevelCount = 1,
				.layerCount = 1,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Depth stencil",
				#endif
			},
			0
		);
	}
}