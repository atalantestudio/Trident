// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/CommandPool.hpp"
#include "Engine/API/Rendering/Queue.hpp"

namespace ProjectA {
	class VK::Queue final : public ProjectA::Queue {
		public:
			explicit Queue(const VK::Device& device, uint32 queueFamilyIndex);

			ref<ProjectA::CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const final override;

			void submit(const sequence<ref<ProjectA::CommandBuffer>>& commandBuffers, ref<ProjectA::Semaphore> waitSemaphore, ref<ProjectA::Semaphore> signalSemaphore, ref<ProjectA::Fence> fence) const final override;

			PresentStatus present(const sequence<ref<ProjectA::Swapchain>>& swapchains, const sequence<uint32>& imageIndices, ref<ProjectA::Semaphore> waitSemaphore) const final override;

			void waitIdle() const final override;

		private:
			VkQueue queue;
			const VK::Device& device;
			VK::CommandPool commandPool;
	};
}