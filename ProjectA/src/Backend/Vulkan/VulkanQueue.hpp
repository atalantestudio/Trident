#pragma once

#include "Backend/Vulkan/VulkanCommandPool.hpp"
#include "Backend/Vulkan/VulkanSwapchain.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Queue.hpp"

namespace ProjectA {
	class VulkanDevice;

	class VulkanQueue : public Queue, public ObjectWrapper<VkQueue> {
		public:
			VulkanQueue(const VulkanDevice& device, uint32 queueFamilyIndex);

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const override;

			void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const override;

			Status present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, uint32 imageIndex) const override;

		private:
			const VulkanDevice& device;
			const VulkanCommandPool commandPool;
	};
}