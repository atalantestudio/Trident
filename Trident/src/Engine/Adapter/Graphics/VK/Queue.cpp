// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/VK/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Fence.hpp"
#include "Engine/Adapter/Graphics/VK/Queue.hpp"
#include "Engine/Adapter/Graphics/VK/Semaphore.hpp"
#include "Engine/Adapter/Graphics/VK/Status.hpp"
#include "Engine/Adapter/Graphics/VK/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Queue::Queue(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) :
		Trident::Queue(device, queueFamilyIndex, queueFamilyCapability)
	{}

	ref<Trident::Queue> VK::createQueue(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);

		ref<VK::Queue> queue = createRef<VK::Queue>(device, queueFamilyIndex, queueFamilyCapability);
		queue->commandPool = createCommandPool(device, queueFamilyIndex, queueFamilyCapability);

		_device->vkGetDeviceQueue(_device->device, queueFamilyIndex, 0, &queue->queue);

		return queue;
	}

	void VK::destroyQueue(const Trident::Queue* queue) {}

	PresentStatus VK::present(const Trident::Queue* queue, view<ref<Trident::Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Trident::Semaphore> waitSemaphore) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Queue* _queue = static_cast<const VK::Queue*>(queue);
		VkResult result;

		// TODO: Move to the graphics adapter.
		// ADAPTER_ASSERT(swapchains.count() == presentableImageIndices.count(), "There must be 1 presentable image index per swapchain.");

		sequence<const VkSwapchainKHR*> vkSwapchains(swapchains.count());
		uint64 swapchainIndex = 0;

		while (swapchainIndex < vkSwapchains.count()) {
			vkSwapchains[swapchainIndex] = &std::static_pointer_cast<VK::Swapchain>(swapchains[swapchainIndex])->swapchain;

			swapchainIndex += 1;
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &std::static_pointer_cast<VK::Semaphore>(waitSemaphore)->semaphore;
		presentInfo.swapchainCount = static_cast<uint32>(vkSwapchains.count());
		presentInfo.pSwapchains = vkSwapchains[0];
		presentInfo.pImageIndices = &presentableImageIndices[0];
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(_queue->queue, &presentInfo);

		return presentStatusFromResult(result);
	}

	void VK::submit(const Trident::Queue* queue, view<ref<Trident::CommandBuffer>> commandBuffers) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Queue* _queue = static_cast<const VK::Queue*>(queue);
		VkResult result;

		sequence<VkCommandBuffer> vkCommandBuffers(commandBuffers.count());
		uint32 commandBufferIndex = 0;

		while (commandBufferIndex < commandBuffers.count()) {
			vkCommandBuffers[commandBufferIndex] = std::static_pointer_cast<VK::CommandBuffer>(commandBuffers[commandBufferIndex])->commandBuffer;

			commandBufferIndex += 1;
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = static_cast<uint32>(vkCommandBuffers.count());
		submitInfo.pCommandBuffers = &vkCommandBuffers[0];
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		result = vkQueueSubmit(_queue->queue, 1, &submitInfo, VK_NULL_HANDLE);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to submit queue ([]).", string_VkResult(result));
	}

	void VK::submit(const Trident::Queue* queue, view<ref<Trident::CommandBuffer>> commandBuffers, ref<Trident::Semaphore> waitSemaphore, ref<Trident::Semaphore> signalSemaphore, ref<Trident::Fence> fence) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Queue* _queue = static_cast<const VK::Queue*>(queue);
		VkResult result;

		// TODO: One wait destination stage mask per wait semaphore, but the pipeline stage flags is stored into the command buffer.
		const VkPipelineStageFlags waitDstStageMask = std::static_pointer_cast<VK::CommandBuffer>(commandBuffers[0])->pipelineStageFlags;

		sequence<VkCommandBuffer> vkCommandBuffers(commandBuffers.count());
		uint32 commandBufferIndex = 0;

		while (commandBufferIndex < commandBuffers.count()) {
			vkCommandBuffers[commandBufferIndex] = std::static_pointer_cast<VK::CommandBuffer>(commandBuffers[commandBufferIndex])->commandBuffer;

			commandBufferIndex += 1;
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

		DEBUG_ASSERT(fence != nullptr);

		result = vkQueueSubmit(_queue->queue, 1, &submitInfo, std::static_pointer_cast<VK::Fence>(fence)->fence);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to submit queue ([]).", string_VkResult(result));
	}

	void VK::waitIdle(const Trident::Queue* queue) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Queue* _queue = static_cast<const VK::Queue*>(queue);
		VkResult result;

		result = vkQueueWaitIdle(_queue->queue);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to wait for queue idle state ([]).", string_VkResult(result));
	}
}