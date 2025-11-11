#include "pch.hpp"

#include "Backend/OpenGL/OpenGLImage.hpp"
#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLSwapchain.hpp"

namespace ProjectA {
	OpenGLSwapchain::OpenGLSwapchain(const SwapchainDescriptor& descriptor, const OpenGLInstance& instance, const OpenGLSurface& surface) :
		Swapchain(descriptor),
		instance(&instance),
		surface(&surface)
	{
		const uint32 imageCount = getImageCount();

		images = sequence<ref<Image>>(imageCount);
		imageViews = sequence<ref<ImageView>>(imageCount);

		update(descriptor);
	}

	uint32 OpenGLSwapchain::getImageCount() const {
		return 1;
	}

	void OpenGLSwapchain::update(const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DEBUG_DESCRIPTOR_GUARD(descriptor);

		extent = descriptor.extent;

		instance->setSwapInterval(static_cast<sint32>(descriptor.vSync));

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = ImageType::TEXTURE_2D;
		imageDescriptor.format = surface->getFormat();
		imageDescriptor.extent.width = extent.width;
		imageDescriptor.extent.height = extent.height;
		imageDescriptor.extent.depth = 1;
		imageDescriptor.mipLevelCount = 1;
		imageDescriptor.layerCount = 1;
		imageDescriptor.sampleCount = 1;

		// Swapchain images are not associated with client-side usage.
		imageDescriptor.usage = static_cast<ImageUsage>(0);

		for (uint32 imageIndex = 0; imageIndex < images.count(); imageIndex += 1) {
			images[imageIndex] = createRef<OpenGLImage>(imageDescriptor);
		}

		createImageViews();
	}
}