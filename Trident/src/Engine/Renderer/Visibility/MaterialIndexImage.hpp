// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Image> VisibilityRenderer::createMaterialIndexImage() const {
		const Extent2D swapchainExtent = getSwapchain()->getExtent();

		ImageDescriptor materialIndexImageDescriptor{};
		materialIndexImageDescriptor.type = ImageType::TEXTURE_2D;
		materialIndexImageDescriptor.format = ImageFormat::R32_UINT;
		materialIndexImageDescriptor.extent.width = swapchainExtent.width;
		materialIndexImageDescriptor.extent.height = swapchainExtent.height;
		materialIndexImageDescriptor.extent.depth = 1;
		materialIndexImageDescriptor.mipLevelCount = 1;
		materialIndexImageDescriptor.layerCount = 1;
		materialIndexImageDescriptor.sampleCount = 1;

		// TODO: Figure out correct usage and memory properties.
		materialIndexImageDescriptor.usage = ImageUsage::STORAGE;
		materialIndexImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		SET_DEBUG_NAME(materialIndexImageDescriptor, "Material index image");

		return createImage(materialIndexImageDescriptor);
	}

	ref<ImageView> VisibilityRenderer::createMaterialIndexImageView() {
		ImageViewDescriptor materialIndexImageViewDescriptor{};
		materialIndexImageViewDescriptor.mipLevelOffset = 0;
		materialIndexImageViewDescriptor.mipLevelCount = 1;
		materialIndexImageViewDescriptor.layerOffset = 0;
		materialIndexImageViewDescriptor.layerCount = 1;

		SET_DEBUG_NAME(materialIndexImageViewDescriptor, "Material index image view");

		return materialIndexImage->createView(materialIndexImageViewDescriptor);
	}
}