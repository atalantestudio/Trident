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
			submitInfo.pWaitSemaphores = &std::static_pointer_cast<VulkanSemaphore>(waitSemaphore)->object;
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
			submitInfo.pSignalSemaphores = &std::static_pointer_cast<VulkanSemaphore>(signalSemaphore)->object;
		}

		VkFence vkFence = VK_NULL_HANDLE;

		if (fence != nullptr) {
			vkFence = std::static_pointer_cast<VulkanFence>(fence)->object;
		}

		const VkResult result = vkQueueSubmit(object, 1, &submitInfo, vkFence);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to submit Vulkan queue ([]).", string_VkResult(result));
	}

	PresentStatus VulkanQueue::present(const std::vector<ref<Swapchain>>& swapchains, const ref<Semaphore>& waitSemaphore, const std::vector<uint32>& imageIndices) const {
		PROFILE_SCOPE(FUNC_SIG);

		// TODO: Find a static way of casting swapchains.
		fchain<const VkSwapchainKHR*> vkSwapchains(swapchains.size());

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
		presentInfo.pImageIndices = imageIndices.data();
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