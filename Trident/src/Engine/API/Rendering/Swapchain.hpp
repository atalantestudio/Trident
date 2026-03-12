// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/SwapchainDescriptor.hpp"
#include "Engine/API/Rendering/Image.hpp"
#include "Engine/API/Rendering/ImageView.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"

namespace Trident {
	class Swapchain {
		public:
			~Swapchain();

			// TODO: Don't expose to the client side.
			void createImageViews() {
				ImageViewDescriptor imageViewDescriptor{};
				imageViewDescriptor.mipLevelOffset = 0;
				imageViewDescriptor.layerOffset = 0;

				uint32 imageViewIndex = 0;

				while (imageViewIndex < imageViews.count()) {
					const ref<Image> image = images[imageViewIndex];

					imageViewDescriptor.mipLevelCount = image->getMipLevelCount();
					imageViewDescriptor.layerCount = image->getLayerCount();

					SET_DEBUG_NAME(imageViewDescriptor, Logger::format("Swapchain image view []", imageViewIndex));

					imageViews[imageViewIndex] = image->createView(imageViewDescriptor);

					imageViewIndex += 1;
				}
			}

			Extent2D getExtent() const;

			uint32 getImageCount() const;

			ref<ImageView> getImageView(uint32 index) const;

			void update(const SwapchainDescriptor& descriptor);

			explicit Swapchain(const Device* device, const SwapchainDescriptor& descriptor);

		// TODO: Don't expose to the client side.
		public:
			const Device* device;
			uint32 imageCount;
			Extent2D extent;
			sequence<ref<Image>> images;
			sequence<ref<ImageView>> imageViews;
	};
}