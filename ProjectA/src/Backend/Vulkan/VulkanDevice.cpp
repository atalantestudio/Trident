#include "pch.hpp"

#include "Backend/Vulkan/Status.hpp"
#include "Backend/Vulkan/VulkanCommandBuffer.hpp"
#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanFence.hpp"
#include "Backend/Vulkan/VulkanSemaphore.hpp"
#include "Backend/Vulkan/VulkanShaderModule.hpp"
#include "Backend/Vulkan/VulkanSwapchain.hpp"

namespace ProjectA {
	// TODO: Client-side extension request.
	const chain<const char*> VulkanDevice::enabledExtensionNames = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface) :
		physicalDevice(physicalDevice.object)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[VulkanDevice]");

		chain<VkDeviceQueueCreateInfo> queueCreateInfos;

		if (physicalDevice.graphicsQueueFamilyIndex == surface.presentQueueFamilyIndex) {
			// Create one queue.
			const float32 queuePriority = 1.0f;

			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.pNext = nullptr;
			queueCreateInfo.flags = 0;
			queueCreateInfo.queueFamilyIndex = physicalDevice.graphicsQueueFamilyIndex;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.emplace_back(queueCreateInfo);
		}
		else {
			// Create a graphics queue and a present queue.
			const float32 graphicsQueuePriority = 1.0f;

			VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
			graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			graphicsQueueCreateInfo.pNext = nullptr;
			graphicsQueueCreateInfo.flags = 0;
			graphicsQueueCreateInfo.queueFamilyIndex = physicalDevice.graphicsQueueFamilyIndex;
			graphicsQueueCreateInfo.queueCount = 1;
			graphicsQueueCreateInfo.pQueuePriorities = &graphicsQueuePriority;

			queueCreateInfos.emplace_back(graphicsQueueCreateInfo);

			const float32 presentQueuePriority = 1.0f;

			VkDeviceQueueCreateInfo presentQueueCreateInfo{};
			presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			presentQueueCreateInfo.pNext = nullptr;
			presentQueueCreateInfo.flags = 0;
			presentQueueCreateInfo.queueFamilyIndex = surface.presentQueueFamilyIndex;
			presentQueueCreateInfo.queueCount = 1;
			presentQueueCreateInfo.pQueuePriorities = &presentQueuePriority;

			queueCreateInfos.emplace_back(presentQueueCreateInfo);
		}

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32>(enabledExtensionNames.size());
		deviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();
		deviceCreateInfo.pEnabledFeatures = &physicalDevice.features;

		if (vkCreateDevice(physicalDevice.object, &deviceCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create device.");
		}

		vkGetDeviceQueue(object, physicalDevice.graphicsQueueFamilyIndex, 0, &graphicsQueue);
		vkGetDeviceQueue(object, surface.presentQueueFamilyIndex, 0, &presentQueue);
	}

	VulkanDevice::~VulkanDevice() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanDevice]");

		vkDestroyDevice(object, nullptr);
	}

	uint32 VulkanDevice::findMemoryType(const VkMemoryRequirements& memoryRequirements, const VkMemoryPropertyFlags memoryPropertyFlags) const {
		PROFILE_SCOPE(FUNC_SIG);

		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties{};

		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

		// Find suitable memory type.
		for (uint32 memoryTypeIndex = 0; memoryTypeIndex < physicalDeviceMemoryProperties.memoryTypeCount; memoryTypeIndex += 1) {
			if ((memoryRequirements.memoryTypeBits & (1 << memoryTypeIndex)) && (physicalDeviceMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
				return memoryTypeIndex;
			}
		}

		throw std::runtime_error("Failed to find a suitable memory type.");
	}

	void VulkanDevice::waitForFence(const ref<Fence>& fence) const {
		PROFILE_SCOPE(FUNC_SIG);

		// UINT64_MAX disables the timeout.
		vkWaitForFences(object, 1, &static_cast<const VulkanFence*>(fence.get())->object, VK_TRUE, UINT64_MAX);
	}

	void VulkanDevice::resetFence(const ref<Fence>& fence) const {
		PROFILE_SCOPE(FUNC_SIG);

		vkResetFences(object, 1, &static_cast<const VulkanFence*>(fence.get())->object);
	}

	Status VulkanDevice::acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& nextImageIndex) const {
		PROFILE_SCOPE(FUNC_SIG);

		// UINT64_MAX disables the timeout.
		const VkResult result = vkAcquireNextImageKHR(
			object,
			static_cast<const VulkanSwapchain*>(swapchain.get())->object,
			UINT64_MAX,
			static_cast<const VulkanSemaphore*>(semaphore.get())->object,
			VK_NULL_HANDLE,
			&nextImageIndex
		);

		return getStatus(result);
	}

	void VulkanDevice::submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const {
		PROFILE_SCOPE(FUNC_SIG);

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

		submitInfo.pWaitDstStageMask = &static_cast<const VulkanCommandBuffer*>(commandBuffer.get())->pipelineStageFlags;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &static_cast<const VulkanCommandBuffer*>(commandBuffer.get())->object;

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

		const VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, vkFence);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to submit queue.");
		}
	}

	Status VulkanDevice::present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, const uint32 imageIndex) const {
		PROFILE_SCOPE(FUNC_SIG);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &static_cast<const VulkanSemaphore*>(waitSemaphore.get())->object;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &static_cast<const VulkanSwapchain*>(swapchain.get())->object;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		const VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);

		return getStatus(result);
	}

	void VulkanDevice::waitIdle() const {
		vkDeviceWaitIdle(object);
	}
}