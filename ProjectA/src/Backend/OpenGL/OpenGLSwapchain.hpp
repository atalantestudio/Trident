#pragma once

#include "Backend/OpenGL/OpenGLImageView.hpp"

#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	class OpenGLSwapchain : public Swapchain {
		public:
			OpenGLSwapchain(const SwapchainDescriptor& descriptor);

			void update(const SwapchainDescriptor& descriptor) final override;

			uint32 getExtentWidth() const final override;
			uint32 getExtentHeight() const final override;

			void construct(const SwapchainDescriptor& descriptor);

			void destroy();

		public:
			const uint32 imageCount = 1;
			chain<ref<OpenGLImage>> images;
			chain<ref<OpenGLImageView>> imageViews;
			chain<GLuint> framebuffers;

		private:
			uint32 extentWidth;
			uint32 extentHeight;
	};
}