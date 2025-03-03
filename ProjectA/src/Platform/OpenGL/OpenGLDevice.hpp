#pragma once

#include "Engine/Renderer/Device.hpp"

namespace ProjectA {
	class OpenGLDevice : public Device {
		public:
			OpenGLDevice();

			void waitForFences() const override;

			void resetFences() const override;

			uint32 acquireNextImage(const ref<Swapchain>& swapchain) const override;

			void submit(const ref<CommandBuffer>& commandBuffer) const override;

			void present(const ref<Swapchain>& swapchain, const uint32 imageIndex) const override;

			void waitIdle() const override;
	};
}