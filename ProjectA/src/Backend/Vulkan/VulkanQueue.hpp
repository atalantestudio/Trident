#pragma once

#include <vulkan/vulkan.h>

#include "Backend/Vulkan/VulkanCommandPool.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Queue.hpp"

namespace ProjectA {
	class VulkanQueue : public Queue, public ObjectWrapper<VkQueue> {
		public:
			VulkanQueue(VkDevice device, uint32 queueFamilyIndex);

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const override;

			void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const override;

		private:
			const VkDevice device;
			const VulkanCommandPool commandPool;
	};
}