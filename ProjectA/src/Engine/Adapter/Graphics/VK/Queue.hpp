#pragma once

#include "Engine/Adapter/Graphics/VK/CommandPool.hpp"
#include "Engine/Adapter/Graphics/VK/Swapchain.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Queue.hpp"

namespace ProjectA {
	class VulkanDevice;

	class VulkanQueue final : public Queue, public ObjectWrapper<VkQueue> {
		public:
			explicit VulkanQueue(const VulkanDevice& device, uint32 queueFamilyIndex);

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const final override;

			void submit(const sequence<ref<CommandBuffer>>& commandBuffers, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const final override;

			PresentStatus present(const sequence<ref<Swapchain>>& swapchains, const ref<Semaphore>& waitSemaphore, const sequence<uint32>& imageIndices) const final override;

			void waitIdle() const final override;

		private:
			const VulkanDevice& device;
			const VulkanCommandPool commandPool;
	};
}