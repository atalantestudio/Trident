#pragma once

#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	class OpenGLSwapchain : public Swapchain {
		public:
			OpenGLSwapchain(const SwapchainDescriptor& descriptor);

			void recreate(const SwapchainDescriptor& descriptor) override;

			uint32 getExtentWidth() const override;
			uint32 getExtentHeight() const override;

		private:
			uint32 extentWidth;
			uint32 extentHeight;
	};
}