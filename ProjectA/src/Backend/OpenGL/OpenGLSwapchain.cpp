#include "pch.hpp"

#include "Backend/OpenGL/OpenGLSwapchain.hpp"

namespace ProjectA {
	OpenGLSwapchain::OpenGLSwapchain(GLFWwindow* window, const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLSwapchain]");

		int32 width;
		int32 height;

		glfwGetWindowSize(window, &width, &height);

		extentWidth = static_cast<uint32>(width);
		extentHeight = static_cast<uint32>(height);
	}

	uint32 OpenGLSwapchain::getExtentWidth() const {
		return extentWidth;
	}

	uint32 OpenGLSwapchain::getExtentHeight() const {
		return extentHeight;
	}
}