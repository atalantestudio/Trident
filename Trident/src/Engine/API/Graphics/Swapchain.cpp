// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/Swapchain.hpp"

namespace Trident {
	#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
		static void validateSwapchainDescriptor(const SwapchainDescriptor& descriptor) {
			uint32 errorCount = 0;

			if (descriptor.extent.width == 0) {
				ENGINE_ERROR("descriptor.extent.width must not be 0.");

				errorCount += 1;
			}

			if (descriptor.extent.height == 0) {
				ENGINE_ERROR("descriptor.extent.height must not be 0.");

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		}
	#endif

	Swapchain::Swapchain(const Device* device, const SwapchainDescriptor& descriptor) :
		device(device),
		extent(descriptor.extent)
	{
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			validateSwapchainDescriptor(descriptor);
		#endif
	}

	Swapchain::~Swapchain() {
		device->adapter->destroySwapchainImplementation(device, this);

		// TODO(graphics/dx12/swapchain-image): The swapchain images are already deleted.
	}

	Extent2D Swapchain::getExtent() const {
		return extent;
	}

	uint32 Swapchain::getImageCount() const {
		return imageCount;
	}

	ref<ImageView> Swapchain::getImageView(uint32 index) const {
		ENGINE_ASSERT(index < imageViews.count(), "Image view index overflow.");

		return imageViews[index];
	}

	void Swapchain::createImageViews() {
		ImageViewDescriptor imageViewDescriptor = {
			.mipLevelOffset = 0,
			.layerOffset = 0,
		};

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> name;
		#endif

		uint32 imageViewIndex = 0;

		while (imageViewIndex < imageViews.count()) {
			const ref<Image> image = images[imageViewIndex];

			imageViewDescriptor.mipLevelCount = image->getMipLevelCount();
			imageViewDescriptor.layerCount = image->getLayerCount();

			#if defined(DEBUG_OBJECT_NAME)
				name = Logger::format("Swapchain view []", imageViewIndex);

				imageViewDescriptor.name = name;
			#endif

			imageViews[imageViewIndex] = image->createView(imageViewDescriptor);

			imageViewIndex += 1;
		}
	}

	void Swapchain::update(const SwapchainDescriptor& descriptor) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			validateSwapchainDescriptor(descriptor);
		#endif

		device->adapter->updateSwapchainImplementation(device, this, descriptor);
	}
}