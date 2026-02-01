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
			virtual ~Swapchain() = default;

			Extent2D getExtent() const {
				return extent;
			}

			virtual uint32 getImageCount() const = 0;

			ref<ImageView> getImageView(uint32 index) const {
				ENGINE_ASSERT(index < imageViews.count(), "Image view index overflow.");

				return imageViews[index];
			}

			virtual void update(const SwapchainDescriptor& descriptor) = 0;

		protected:
			explicit Swapchain(const SwapchainDescriptor& descriptor) :
				extent(descriptor.extent)
			{
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}

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

		protected:
			Extent2D extent;
			sequence<ref<Image>> images;
			sequence<ref<ImageView>> imageViews;
	};
}