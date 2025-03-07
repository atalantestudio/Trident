#pragma once

#include <glfw/glfw3.h>

#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	class OpenGLSwapchain : public Swapchain {
		public:
			OpenGLSwapchain(GLFWwindow* window, const SwapchainDescriptor& descriptor);

			uint32 getExtentWidth() const override;
			uint32 getExtentHeight() const override;

		private:
			uint32 extentWidth;
			uint32 extentHeight;
	};
}