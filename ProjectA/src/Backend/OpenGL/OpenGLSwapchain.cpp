#include "pch.hpp"

#include "Backend/OpenGL/OpenGLSwapchain.hpp"

namespace ProjectA {
	OpenGLSwapchain::OpenGLSwapchain(const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		recreate(descriptor);
	}

	void OpenGLSwapchain::recreate(const SwapchainDescriptor& descriptor) {
		#ifdef BUILD_MODE_DEBUG
			ENGINE_ASSERT(descriptor.extentWidth != 0 && descriptor.extentWidth != std::numeric_limits<uint32>::max(), "Unusual extent width value.");
			ENGINE_ASSERT(descriptor.extentHeight != 0 && descriptor.extentHeight != std::numeric_limits<uint32>::max(), "Unusual extent height value.");
		#endif

		extentWidth = descriptor.extentWidth;
		extentHeight = descriptor.extentHeight;
	}

	uint32 OpenGLSwapchain::getExtentWidth() const {
		return extentWidth;
	}

	uint32 OpenGLSwapchain::getExtentHeight() const {
		return extentHeight;
	}
}