// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<Framebuffer> Lightmapper::createGBufferFramebuffer() const {
		return device->createFramebuffer({
			.renderPass = gBufferRenderPass,
			.imageViews = {
				context.gBufferPositionImageView,
				context.gBufferSmoothPositionImageView,
				context.gBufferTriangleNormalImageView,
				context.gBufferVertexNormalImageView,
			},
			.extent = {imageAssetDescriptor.extent.width, imageAssetDescriptor.extent.height},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "G-Buffer",
			#endif
		});
	}
}