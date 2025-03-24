#include "pch.hpp"

#include "Backend/OpenGL/OpenGLSwapchain.hpp"

namespace ProjectA {
	OpenGLSwapchain::OpenGLSwapchain(const SwapchainDescriptor& descriptor) :
		extentWidth(descriptor.extentWidth),
		extentHeight(descriptor.extentHeight)
	{
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLSwapchain]");

		recreate(descriptor);
	}

	void OpenGLSwapchain::recreate(const SwapchainDescriptor& descriptor) {}

	uint32 OpenGLSwapchain::getExtentWidth() const {
		return extentWidth;
	}

	uint32 OpenGLSwapchain::getExtentHeight() const {
		return extentHeight;
	}
}