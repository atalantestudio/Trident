#pragma once

#include "Engine/Renderer/CommandBuffer.hpp"
#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	class Device {
		public:
			virtual ~Device() = default;

			virtual void waitForFences() const = 0;

			virtual void resetFences() const = 0;

			// Returns the index of the next available swapchain image.
			virtual uint32 acquireNextImage(const ref<Swapchain>& swapchain) const = 0;

			virtual void submit(const ref<CommandBuffer>& commandBuffer) const = 0;

			virtual void present(const ref<Swapchain>& swapchain, const uint32 imageIndex) const = 0;

			virtual void waitIdle() const = 0;
	};
}