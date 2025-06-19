#pragma once

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

		private:
			const OpenGLRenderingContext* renderingContext;
	};
}