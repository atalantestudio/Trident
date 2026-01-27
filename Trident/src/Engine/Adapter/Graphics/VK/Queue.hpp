// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/CommandPool.hpp"
#include "Engine/API/Rendering/Queue.hpp"

namespace Trident {
	class VK::Queue final : public Trident::Queue {
		public:
			explicit Queue(const VK::Device& device, uint32 queueFamilyIndex);

			ref<Trident::CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const final override;

			void submit(view<ref<Trident::CommandBuffer>> commandBuffers, ref<Trident::Semaphore> waitSemaphore, ref<Trident::Semaphore> signalSemaphore, ref<Trident::Fence> fence) const final override;

			PresentStatus present(view<ref<Trident::Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Trident::Semaphore> waitSemaphore) const final override;

			void waitIdle() const final override;

		private:
			VkQueue queue;
			const VK::Device& device;
			VK::CommandPool commandPool;
	};
}