#include "pch.hpp"

#include "Backend/OpenGL/OpenGLImage.hpp"
#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLSwapchain.hpp"

namespace ProjectA {
	OpenGLSwapchain::OpenGLSwapchain(const SwapchainDescriptor& descriptor, const OpenGLRenderingContext& renderingContext, const OpenGLSurface& surface) :
		renderingContext(&renderingContext),
		surface(&surface)
	{
		images = sequence<ref<Image>>(1);
		imageViews = sequence<ref<ImageView>>(1);

		construct(descriptor);
	}

	uint32 OpenGLSwapchain::getImageCount() const {
		return 1;
	}

	void OpenGLSwapchain::update(const SwapchainDescriptor& descriptor) {
		construct(descriptor);
	}

	void OpenGLSwapchain::construct(const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		DEBUG_ASSERT(descriptor.extent.width > 0);
		DEBUG_ASSERT(descriptor.extent.height > 0);

		extent = descriptor.extent;

		renderingContext->setSwapInterval(static_cast<sint32>(descriptor.vSync));

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

		for (uint32 imageIndex = 0; imageIndex < images.count; imageIndex += 1) {
			images[imageIndex] = createRef<OpenGLImage>(imageDescriptor);
		}

		createImageViews();
	}
}