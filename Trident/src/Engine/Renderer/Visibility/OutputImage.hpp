// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<Image> VisibilityRenderer::createOutputImage() const {
		const Extent2D extent = getSwapchain()->getExtent();

		ImageDescriptor outputImageDescriptor{};
		outputImageDescriptor.type = ImageType::TEXTURE_2D;
		outputImageDescriptor.format = getSurface()->getFormat();
		outputImageDescriptor.extent.width = extent.width;
		outputImageDescriptor.extent.height = extent.height;
		outputImageDescriptor.extent.depth = 1;
		outputImageDescriptor.mipLevelCount = 1;
		outputImageDescriptor.layerCount = 1;
		outputImageDescriptor.sampleCount = 1;
		outputImageDescriptor.usage = ImageUsage::STORAGE;
		outputImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		SET_DEBUG_NAME(outputImageDescriptor, "Output");

		return createImage(outputImageDescriptor);
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