// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/Swapchain.hpp"

namespace Trident {
	GL::Swapchain::Swapchain(const Trident::Device* device, const SwapchainDescriptor& descriptor) :
		Trident::Swapchain(device, descriptor)
	{}

	ref<Trident::Swapchain> GL::createSwapchain(const Trident::Device* device, const SwapchainDescriptor& descriptor) {
		static constexpr uint32 IMAGE_COUNT = 3;

		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Swapchain> swapchain = allocateRef<GL::Swapchain>(device, descriptor);
		swapchain->imageCount = IMAGE_COUNT;
		swapchain->images = sequence<private_ref<Trident::Image>>(swapchain->imageCount);
		swapchain->imageViews = sequence<private_ref<Trident::ImageView>>(swapchain->imageCount);

		updateSwapchain(device, swapchain, descriptor);

		return swapchain;
	}

	void GL::destroySwapchain(const Trident::Device* device, const Trident::Swapchain* swapchain) {}

	void GL::recreateSwapchain(const Trident::Device* device, Trident::Swapchain* swapchain, const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		updateSwapchain(device, swapchain, descriptor);
	}

	void GL::updateSwapchain(const Trident::Device* device, Trident::Swapchain* swapchain, const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		swapchain->extent = descriptor.extent;

		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
			static const PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = loadProcedure<PFNWGLGETEXTENSIONSSTRINGEXTPROC>("wglGetExtensionsStringEXT");
			static const view<char8> extensionsString = wglGetExtensionsStringEXT();

			ADAPTER_ASSERT(extensionsString.find("WGL_EXT_swap_control", 0) < extensionsString.count(), "WGL_EXT_swap_control extension not supported.");

			static const PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = loadProcedure<PFNWGLSWAPINTERVALEXTPROC>("wglSwapIntervalEXT");

			BOOL result;

			result = wglSwapIntervalEXT(static_cast<sint32>(descriptor.verticalSync));

			WIN32_ASSERT(result == TRUE);
		#endif

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = ImageType::TEXTURE_2D;
		imageDescriptor.format = descriptor.surface->getFormat();
		imageDescriptor.extent.width = swapchain->extent.width;
		imageDescriptor.extent.height = swapchain->extent.height;
		imageDescriptor.extent.depth = 1;
		imageDescriptor.mipLevelCount = 1;
		imageDescriptor.layerCount = 1;
		imageDescriptor.sampleCount = 1;
		imageDescriptor.usage = static_cast<ImageUsage>(0);

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> name;
		#endif

		uint32 imageIndex = 0;

		while (imageIndex < swapchain->images.count()) {
			#if defined(DEBUG_OBJECT_NAME)
				name = Logger::format("Swapchain []", imageIndex);

				imageDescriptor.name = name;
			#endif

			swapchain->images[imageIndex] = createImage(device, imageDescriptor);
			swapchain->images[imageIndex]->memorySuballocation.allocationIndex = 0;

			imageIndex += 1;
		}

		swapchain->createImageViews();
	}
}