// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/VK/CommandEncoder.hpp"
// #include "Engine/Adapter/Graphics/VK/CommandPool.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Fence.hpp"
#include "Engine/Adapter/Graphics/VK/Queue.hpp"
#include "Engine/Adapter/Graphics/VK/Semaphore.hpp"
#include "Engine/Adapter/Graphics/VK/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	VK::Queue::Queue(const VK::Device& device, uint32 queueFamilyIndex) :
		device(device),
		commandPool(device, queueFamilyIndex)
	{
		vkGetDeviceQueue(device.device, queueFamilyIndex, 0, &queue);
	}

	ref<CommandEncoder> VK::Queue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createRef<VK::CommandEncoder>(descriptor, device, commandPool);
	}

	void VK::Queue::submit(const sequence<ref<ProjectA::CommandBuffer>>& commandBuffers, ref<ProjectA::Semaphore> waitSemaphore, ref<ProjectA::Semaphore> signalSemaphore, ref<ProjectA::Fence> fence) const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		// TODO: One wait destination stage mask per wait semaphore, but the pipeline stage flags is stored into the command buffer.
		const VkPipelineStageFlags waitDstStageMask = std::static_pointer_cast<VK::CommandBuffer>(commandBuffers[0])->pipelineStageFlags;

		sequence<VkCommandBuffer> vkCommandBuffers(commandBuffers.count());

		for (uint32 commandBufferIndex = 0; commandBufferIndex < commandBuffers.count(); commandBufferIndex += 1) {
			vkCommandBuffers[commandBufferIndex] = std::static_pointer_cast<VK::CommandBuffer>(commandBuffers[commandBufferIndex])->commandBuffer;
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
			submitInfo.pWaitSemaphores = &std::static_pointer_cast<VK::Semaphore>(waitSemaphore)->semaphore;
			submitInfo.pWaitDstStageMask = &waitDstStageMask;
		}

		submitInfo.commandBufferCount = static_cast<uint32>(vkCommandBuffers.count());
		submitInfo.pCommandBuffers = &vkCommandBuffers[0];

		if (signalSemaphore == nullptr) {
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;
		}
		else {
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &std::static_pointer_cast<VK::Semaphore>(signalSemaphore)->semaphore;
		}

		VkFence vkFence = VK_NULL_HANDLE;

		if (fence != nullptr) {
			vkFence = std::static_pointer_cast<VK::Fence>(fence)->fence;
		}

		const VkResult result = vkQueueSubmit(queue, 1, &submitInfo, vkFence);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to submit queue ([]).", string_VkResult(result));
	}

	PresentStatus VK::Queue::present(const sequence<ref<ProjectA::Swapchain>>& swapchains, ref<ProjectA::Semaphore> waitSemaphore, const sequence<uint32>& imageIndices) const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		sequence<const VkSwapchainKHR*> vkSwapchains(swapchains.count());

		for (uint64 swapchainIndex = 0; swapchainIndex < vkSwapchains.count(); swapchainIndex += 1) {
			vkSwapchains[swapchainIndex] = &std::static_pointer_cast<VK::Swapchain>(swapchains[swapchainIndex])->swapchain;
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &std::static_pointer_cast<VK::Semaphore>(waitSemaphore)->semaphore;
		presentInfo.swapchainCount = static_cast<uint32>(vkSwapchains.count());
		presentInfo.pSwapchains = vkSwapchains[0];
		presentInfo.pImageIndices = &imageIndices[0];
		presentInfo.pResults = nullptr;

		const VkResult result = vkQueuePresentKHR(queue, &presentInfo);

		if (result == VK_SUCCESS) {
			return PresentStatus::SUCCESS;
		}

		if (result == VK_SUBOPTIMAL_KHR) {
			return PresentStatus::SUBOPTIMAL;
		}

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			return PresentStatus::ERROR_OUT_OF_DATE;
		}

		ADAPTER_ASSERT(false, "Unhandled result.");

		return PresentStatus::ERROR_UNHANDLED;
	}

	void VK::Queue::waitIdle() const {
		const VkResult result = vkQueueWaitIdle(queue);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to wait for queue idle state ([]).", string_VkResult(result));
	}
}