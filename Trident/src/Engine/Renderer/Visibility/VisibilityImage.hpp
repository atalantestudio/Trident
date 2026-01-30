// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Image> VisibilityRenderer::createVisibilityImage() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		AttachmentImageDescriptor visibilityAttachmentImageDescriptor{};
		visibilityAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		visibilityAttachmentImageDescriptor.extent.width = swapchainExtent.width;
		visibilityAttachmentImageDescriptor.extent.height = swapchainExtent.height;
		visibilityAttachmentImageDescriptor.extent.depth = 1;
		visibilityAttachmentImageDescriptor.mipLevelCount = 1;
		visibilityAttachmentImageDescriptor.layerCount = 1;

		SET_DEBUG_NAME(visibilityAttachmentImageDescriptor, "Visibility image");

		return renderPass->createInputAttachmentImage(visibilityAttachmentImageDescriptor, 1);
	}

	ref<ImageView> VisibilityRenderer::createVisibilityImageView() {
		ImageViewDescriptor visibilityImageViewDescriptor{};
		visibilityImageViewDescriptor.mipLevelOffset = 0;
		visibilityImageViewDescriptor.mipLevelCount = 1;
		visibilityImageViewDescriptor.layerOffset = 0;
		visibilityImageViewDescriptor.layerCount = 1;

		SET_DEBUG_NAME(visibilityImageViewDescriptor, "Visibility image view");

		return visibilityImage->createView(visibilityImageViewDescriptor);
	}
}