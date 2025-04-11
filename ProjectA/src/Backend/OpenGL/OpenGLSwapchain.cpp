#include "pch.hpp"

#include "Backend/OpenGL/OpenGLSwapchain.hpp"

namespace ProjectA {
	OpenGLSwapchain::OpenGLSwapchain(const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		recreate(descriptor);
	}

	void OpenGLSwapchain::recreate(const SwapchainDescriptor& descriptor) {
		// TODO
		//extentWidth = descriptor.extentWidth;
		//extentHeight = descriptor.extentHeight;
	}

	uint32 OpenGLSwapchain::getExtentWidth() const {
		return extentWidth;
	}

	uint32 OpenGLSwapchain::getExtentHeight() const {
		return extentHeight;
	}
}