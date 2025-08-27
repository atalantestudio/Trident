#include "pch.hpp"

#include "Backend/Vulkan/VulkanCommandBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandEncoder.hpp"
#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanFence.hpp"
#include "Backend/Vulkan/VulkanQueue.hpp"
#include "Backend/Vulkan/VulkanSemaphore.hpp"

namespace ProjectA {
	VulkanQueue::VulkanQueue(const VulkanDevice& device, uint32 queueFamilyIndex) :
		device(device),
		commandPool(device.object, queueFamilyIndex)
	{
		vkGetDeviceQueue(device.object, queueFamilyIndex, 0, &object);
	}

	ref<CommandEncoder> VulkanQueue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createRef<VulkanCommandEncoder>(device, commandPool.object, descriptor);
	}

	void VulkanQueue::submit(const sequence<ref<CommandBuffer>>& commandBuffers, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		// TODO: One wait destination stage mask per wait semaphore, but the pipeline stage flags is stored into the command buffer.
		const VkPipelineStageFlags waitDstStageMask = std::static_pointer_cast<VulkanCommandBuffer>(commandBuffers[0])->pipelineStageFlags;

		sequence<VkCommandBuffer> vkCommandBuffers(commandBuffers.count);

		for (uint32 commandBufferIndex = 0; commandBufferIndex < commandBuffers.count; commandBufferIndex += 1) {
			vkCommandBuffers[commandBufferIndex] = std::static_pointer_cast<VulkanCommandBuffer>(commandBuffers[commandBufferIndex])->object;
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;

		if (waitSemaphore == nullptr) {
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
		}
		else {
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &std::static_pointer_cast<VulkanSemaphore>(waitSemaphore)->object;
			submitInfo.pWaitDstStageMask = &waitDstStageMask;
		}

		submitInfo.commandBufferCount = static_cast<uint32>(vkCommandBuffers.count);
		submitInfo.pCommandBuffers = vkCommandBuffers;

		if (signalSemaphore == nullptr) {
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;
		}
		else {
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &std::static_pointer_cast<VulkanSemaphore>(signalSemaphore)->object;
		}

		VkFence vkFence = VK_NULL_HANDLE;

		if (fence != nullptr) {
			vkFence = std::static_pointer_cast<VulkanFence>(fence)->object;
		}

		const VkResult result = vkQueueSubmit(object, 1, &submitInfo, vkFence);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to submit Vulkan queue ([]).", string_VkResult(result));
	}

	PresentStatus VulkanQueue::present(const sequence<ref<Swapchain>>& swapchains, const ref<Semaphore>& waitSemaphore, const sequence<uint32>& imageIndices) const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		// TODO: Speed this up.
		sequence<const VkSwapchainKHR*> vkSwapchains(swapchains.count);

		for (uint64 swapchainIndex = 0; swapchainIndex < vkSwapchains.count; swapchainIndex += 1) {
			vkSwapchains[swapchainIndex] = &std::static_pointer_cast<VulkanSwapchain>(swapchains[swapchainIndex])->object;
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &std::static_pointer_cast<VulkanSemaphore>(waitSemaphore)->object;
		presentInfo.swapchainCount = static_cast<uint32>(vkSwapchains.count);
		presentInfo.pSwapchains = vkSwapchains[0];
		presentInfo.pImageIndices = imageIndices;
		presentInfo.pResults = nullptr;

		const VkResult result = vkQueuePresentKHR(object, &presentInfo);

		if (result == VK_SUCCESS) {
			return PresentStatus::SUCCESS;
		}

		if (result == VK_SUBOPTIMAL_KHR) {
			return PresentStatus::SUBOPTIMAL;
		}

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			return PresentStatus::ERROR_OUT_OF_DATE;
		}

		DEBUG_ASSERT(false);

		return PresentStatus::ERROR_UNHANDLED;
	}
}