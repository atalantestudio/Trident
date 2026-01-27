// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/Swapchain.hpp"

namespace Trident {
	GL::Swapchain::Swapchain(const SwapchainDescriptor& descriptor, const GL::Instance& instance, const GL::Surface& surface) :
		Trident::Swapchain(descriptor),
		instance(instance),
		surface(surface)
	{
		const uint32 imageCount = getImageCount();

		images = sequence<ref<Trident::Image>>(imageCount);
		imageViews = sequence<ref<Trident::ImageView>>(imageCount);

		update(descriptor);
	}

	uint32 GL::Swapchain::getImageCount() const {
		return 1;
	}

	void GL::Swapchain::update(const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DEBUG_DESCRIPTOR_GUARD(descriptor);

		extent = descriptor.extent;

		instance.setSwapInterval(static_cast<sint32>(descriptor.vSync));

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = ImageType::TEXTURE_2D;
		imageDescriptor.format = surface.getFormat();
		imageDescriptor.extent.width = extent.width;
		imageDescriptor.extent.height = extent.height;
		imageDescriptor.extent.depth = 1;
		imageDescriptor.mipLevelCount = 1;
		imageDescriptor.layerCount = 1;
		imageDescriptor.sampleCount = 1;

		// Swapchain images are not associated with client-side usage.
		imageDescriptor.usage = static_cast<ImageUsage>(0);

		for (uint32 imageIndex = 0; imageIndex < images.count(); imageIndex += 1) {
			images[imageIndex] = createRef<GL::Image>(imageDescriptor);
		}

		createImageViews();
	}
}