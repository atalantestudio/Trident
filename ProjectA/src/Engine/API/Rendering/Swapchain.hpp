#pragma once

#include "Engine/API/Rendering/Descriptor/SwapchainDescriptor.hpp"
#include "Engine/API/Rendering/Image.hpp"
#include "Engine/API/Rendering/ImageView.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class Swapchain {
		public:
			virtual ~Swapchain() = default;

			Extent2D getExtent() const {
				return extent;
			}

			virtual uint32 getImageCount() const = 0;

			const ref<ImageView>& getImageView(uint32 index) const {
				ENGINE_ASSERT(index < imageViews.count, "Image view index overflow.");

				return imageViews[index];
			}

			virtual void update(const SwapchainDescriptor& descriptor) = 0;

		protected:
			void createImageViews() {
				ImageViewDescriptor imageViewDescriptor{};
				imageViewDescriptor.mipLevelOffset = 0;
				imageViewDescriptor.layerOffset = 0;

				for (uint32 imageViewIndex = 0; imageViewIndex < imageViews.count; imageViewIndex += 1) {
					const ref<Image>& image = images[imageViewIndex];

					imageViewDescriptor.mipLevelCount = image->getMipLevelCount();
					imageViewDescriptor.layerCount = image->getLayerCount();

					imageViews[imageViewIndex] = images[imageViewIndex]->createView(imageViewDescriptor);
				}
			}

		protected:
			Extent2D extent;
			sequence<ref<Image>> images;
			sequence<ref<ImageView>> imageViews;
	};
}