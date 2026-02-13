// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Image> VisibilityRenderer::createOutputImage() const {
		const Extent2D extent = getSwapchain()->getExtent();

		AttachmentImageDescriptor outputAttachmentImageDescriptor{};
		outputAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		outputAttachmentImageDescriptor.extent.width = extent.width;
		outputAttachmentImageDescriptor.extent.height = extent.height;
		outputAttachmentImageDescriptor.extent.depth = 1;
		outputAttachmentImageDescriptor.mipLevelCount = 1;
		outputAttachmentImageDescriptor.layerCount = 1;
		outputAttachmentImageDescriptor.additionalUsage = ImageUsage::STORAGE;

		SET_DEBUG_NAME(outputAttachmentImageDescriptor, "Output");

		return visibilityRenderPass->createColorAttachmentImage(outputAttachmentImageDescriptor, 1);
	}

	ref<ImageView> VisibilityRenderer::createOutputImageView() {
		ImageViewDescriptor outputImageViewDescriptor{};
		outputImageViewDescriptor.mipLevelOffset = 0;
		outputImageViewDescriptor.mipLevelCount = 1;
		outputImageViewDescriptor.layerOffset = 0;
		outputImageViewDescriptor.layerCount = 1;

		SET_DEBUG_NAME(outputImageViewDescriptor, "Output");

		return outputImage->createView(outputImageViewDescriptor);
	}
}