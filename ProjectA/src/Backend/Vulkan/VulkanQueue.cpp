#include "pch.hpp"

#include "Backend/Vulkan/VulkanCommandBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandEncoder.hpp"
#include "Backend/Vulkan/VulkanFence.hpp"
#include "Backend/Vulkan/VulkanQueue.hpp"
#include "Backend/Vulkan/VulkanSemaphore.hpp"

namespace ProjectA {
	VulkanQueue::VulkanQueue(VkDevice device, uint32 queueFamilyIndex) :
		device(device),
		commandPool(device, queueFamilyIndex)
	{
		vkGetDeviceQueue(device, queueFamilyIndex, 0, &object);
	}

	ref<CommandEncoder> VulkanQueue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return createRef<VulkanCommandEncoder>(device, commandPool.object, descriptor);
	}

	void VulkanQueue::submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const {
		PROFILE_SCOPE(FUNC_SIG);

		const ref<VulkanCommandBuffer>& vkCommandBuffer = std::static_pointer_cast<VulkanCommandBuffer>(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;

		if (waitSemaphore == nullptr) {
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
		}
		else {
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &static_cast<const VulkanSemaphore*>(waitSemaphore.get())->object;
		}

		submitInfo.pWaitDstStageMask = &vkCommandBuffer->pipelineStageFlags;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vkCommandBuffer->object;

		if (signalSemaphore == nullptr) {
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;
		}
		else {
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &static_cast<const VulkanSemaphore*>(signalSemaphore.get())->object;
		}

		VkFence vkFence = VK_NULL_HANDLE;

		if (fence != nullptr) {
			vkFence = static_cast<const VulkanFence*>(fence.get())->object;
		}

		const VkResult result = vkQueueSubmit(object, 1, &submitInfo, vkFence);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to submit Vulkan queue (code: []).", result);
	}
}