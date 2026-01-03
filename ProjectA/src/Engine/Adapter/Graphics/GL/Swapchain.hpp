#pragma once

#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/Adapter/Graphics/GL/Surface.hpp"

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