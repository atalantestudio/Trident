#pragma once

#include <glfw/glfw3.h>

#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	class OpenGLSwapchain : public Swapchain {
		public:
			OpenGLSwapchain(const SwapchainDescriptor& descriptor);

			void recreate(const SwapchainDescriptor& descriptor) override;

			uint32 getExtentWidth() const override;
			uint32 getExtentHeight() const override;

		private:
			const uint32 extentWidth;
			const uint32 extentHeight;
	};
}