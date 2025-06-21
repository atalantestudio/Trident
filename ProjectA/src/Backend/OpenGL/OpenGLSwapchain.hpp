#pragma once

#include "Backend/OpenGL/OpenGLRenderingContext.hpp"
#include "Backend/OpenGL/OpenGLSurface.hpp"

#include "Engine/API/Rendering/Swapchain.hpp"

namespace ProjectA {
	class OpenGLSwapchain final : public Swapchain {
		public:
			explicit OpenGLSwapchain(const SwapchainDescriptor& descriptor, const OpenGLRenderingContext& renderingContext, const OpenGLSurface& surface);

			uint32 getImageCount() const final override;

			void update(const SwapchainDescriptor& descriptor) final override;

		private:
			const OpenGLRenderingContext* renderingContext;
			const OpenGLSurface* surface;
	};
}