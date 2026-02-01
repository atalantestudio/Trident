// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Image> VisibilityRenderer::createMaterialIndexImage() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		AttachmentImageDescriptor materialIndexAttachmentImageDescriptor{};
		materialIndexAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		materialIndexAttachmentImageDescriptor.extent.width = swapchainExtent.width;
		materialIndexAttachmentImageDescriptor.extent.height = swapchainExtent.height;
		materialIndexAttachmentImageDescriptor.extent.depth = 1;
		materialIndexAttachmentImageDescriptor.mipLevelCount = 1;
		materialIndexAttachmentImageDescriptor.layerCount = 1;

		SET_DEBUG_NAME(materialIndexAttachmentImageDescriptor, "Material index");

		return renderPass->createInputAttachmentImage(materialIndexAttachmentImageDescriptor, MATERIAL_INDEX_ATTACHMENT_INDEX);
	}

	ref<ImageView> VisibilityRenderer::createMaterialIndexImageView() {
		ImageViewDescriptor materialIndexImageViewDescriptor{};
		materialIndexImageViewDescriptor.mipLevelOffset = 0;
		materialIndexImageViewDescriptor.mipLevelCount = 1;
		materialIndexImageViewDescriptor.layerOffset = 0;
		materialIndexImageViewDescriptor.layerCount = 1;

		SET_DEBUG_NAME(materialIndexImageViewDescriptor, "Material index");

		return materialIndexImage->createView(materialIndexImageViewDescriptor);
	}
}