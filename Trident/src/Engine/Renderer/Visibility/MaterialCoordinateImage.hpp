// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Image> VisibilityRenderer::createMaterialCoordinateImage() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		AttachmentImageDescriptor materialCoordinateAttachmentImageDescriptor{};
		materialCoordinateAttachmentImageDescriptor.type = ImageType::TEXTURE_2D;
		materialCoordinateAttachmentImageDescriptor.extent.width = swapchainExtent.width;
		materialCoordinateAttachmentImageDescriptor.extent.height = swapchainExtent.height;
		materialCoordinateAttachmentImageDescriptor.extent.depth = 1;
		materialCoordinateAttachmentImageDescriptor.mipLevelCount = 1;
		materialCoordinateAttachmentImageDescriptor.layerCount = 1;

		SET_DEBUG_NAME(materialCoordinateAttachmentImageDescriptor, "Material coordinate");

		return renderPass->createInputAttachmentImage(materialCoordinateAttachmentImageDescriptor, MATERIAL_COORDINATE_ATTACHMENT_INDEX);
	}

	ref<ImageView> VisibilityRenderer::createMaterialCoordinateImageView() {
		ImageViewDescriptor materialCoordinateImageViewDescriptor{};
		materialCoordinateImageViewDescriptor.mipLevelOffset = 0;
		materialCoordinateImageViewDescriptor.mipLevelCount = 1;
		materialCoordinateImageViewDescriptor.layerOffset = 0;
		materialCoordinateImageViewDescriptor.layerCount = 1;

		SET_DEBUG_NAME(materialCoordinateImageViewDescriptor, "Material coordinate");

		return materialCoordinateImage->createView(materialCoordinateImageViewDescriptor);
	}
}