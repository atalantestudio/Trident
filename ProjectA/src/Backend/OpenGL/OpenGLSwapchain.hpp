#pragma once

#include "Backend/OpenGL/OpenGLImageView.hpp"

#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	class OpenGLSwapchain : public Swapchain {
		public:
			OpenGLSwapchain(const SwapchainDescriptor& descriptor);

			void recreate(const SwapchainDescriptor& descriptor) final override;

			uint32 getExtentWidth() const final override;
			uint32 getExtentHeight() const final override;

			void construct(const SwapchainDescriptor& descriptor);

			void destroy();

			bool requiresResolvePass() const;

		public:
			chain<GLuint> framebuffers;
			chain<scope<OpenGLImage>> images;
			chain<scope<OpenGLImageView>> imageViews;

		private:
			uint32 extentWidth;
			uint32 extentHeight;
	};
}