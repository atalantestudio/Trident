#pragma once

#include "Backend/OpenGL/OpenGLImageView.hpp"

#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	class OpenGLSwapchain : public Swapchain {
		public:
			OpenGLSwapchain(const SwapchainDescriptor& descriptor);

			uint32 getExtentWidth() const final override;

			uint32 getExtentHeight() const final override;

			void getImages(const ref<RenderPass>& renderPass) final override;

			void update(const SwapchainDescriptor& descriptor) final override;

			void construct(const SwapchainDescriptor& descriptor);

			void destroy();

		public:
			const uint32 imageCount = 1;
			std::vector<ref<OpenGLImage>> images;
			std::vector<ref<OpenGLImageView>> imageViews;
			std::vector<GLuint> framebuffers;

		private:
			uint32 extentWidth;
			uint32 extentHeight;
	};
}