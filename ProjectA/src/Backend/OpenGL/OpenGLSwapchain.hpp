#pragma once

#include "Backend/OpenGL/OpenGLImageView.hpp"
#include "Backend/OpenGL/OpenGLRenderingContext.hpp"

#include "Engine/API/Rendering/Swapchain.hpp"

namespace ProjectA {
	class OpenGLSwapchain : public Swapchain {
		public:
			OpenGLSwapchain(const SwapchainDescriptor& descriptor, const OpenGLRenderingContext& renderingContext);

			uint32 getImageCount() const final override;

			void getImages(const ref<RenderPass>& renderPass) final override;

			void update(const SwapchainDescriptor& descriptor) final override;

			void construct(const SwapchainDescriptor& descriptor);

			void destroy();

		private:
			static constexpr uint32 imageCount = 1;

		public:
			std::vector<ref<OpenGLImage>> images;
			std::vector<ref<OpenGLImageView>> imageViews;
			std::vector<GLuint> framebuffers;

		private:
			const OpenGLRenderingContext* renderingContext;
	};
}