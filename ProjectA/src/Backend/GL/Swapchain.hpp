#pragma once

#include "Backend/GL/Instance.hpp"
#include "Backend/GL/Surface.hpp"

#include "Engine/API/Rendering/Swapchain.hpp"

namespace ProjectA {
	class OpenGLSwapchain final : public Swapchain {
		public:
			explicit OpenGLSwapchain(const SwapchainDescriptor& descriptor, const OpenGLInstance& instance, const OpenGLSurface& surface);

			uint32 getImageCount() const final override;

			void update(const SwapchainDescriptor& descriptor) final override;

		private:
			const OpenGLInstance* instance;
			const OpenGLSurface* surface;
	};
}