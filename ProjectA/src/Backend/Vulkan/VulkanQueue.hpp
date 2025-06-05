#pragma once

#include "Backend/Vulkan/VulkanCommandPool.hpp"
#include "Backend/Vulkan/VulkanSwapchain.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Queue.hpp"

namespace ProjectA {
	class VulkanDevice;

	class VulkanQueue : public Queue, public ObjectWrapper<VkQueue> {
		public:
			VulkanQueue(const VulkanDevice& device, uint32 queueFamilyIndex);

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const final override;

			void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const final override;

			PresentStatus present(const std::vector<ref<Swapchain>>& swapchains, const ref<Semaphore>& waitSemaphore, const std::vector<uint32>& imageIndices) const final override;

		private:
			const VulkanDevice& device;
			const VulkanCommandPool commandPool;
	};
}