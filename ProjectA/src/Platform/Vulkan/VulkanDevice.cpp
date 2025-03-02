#include "pch.hpp"

#include "Platform/Vulkan/VulkanCommandBuffer.hpp"
#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanShaderModule.hpp"
#include "Platform/Vulkan/VulkanSwapchain.hpp"

namespace ProjectA {
	// TODO: Pass enabled extension names as parameter.
	const std::vector<const char*> VulkanDevice::enabledExtensionNames = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface) :
		physicalDevice(physicalDevice.object),
		memory(nullptr)
	{
		PROFILE_SCOPE(FUNC_SIG);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

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

		acquireNextImageSemaphore = createRef<VulkanSemaphore>(object);
		signalSemaphore = createRef<VulkanSemaphore>(object);
		flightFence = createRef<VulkanFence>(object);
	}

	VulkanDevice::~VulkanDevice() {
		PROFILE_SCOPE(FUNC_SIG);

		flightFence.~shared_ptr();
		signalSemaphore.~shared_ptr();
		acquireNextImageSemaphore.~shared_ptr();

		vkFreeMemory(object, memory, nullptr);
		vkDestroyDevice(object, nullptr);
	}

	void VulkanDevice::allocateMemory(const VkMemoryRequirements& memoryRequirements) {
		PROFILE_SCOPE(FUNC_SIG);

		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties{};

		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

		const VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		uint32 memoryTypeIndex = 0;
		bool found = false;

		// Find suitable memory type.
		for (; memoryTypeIndex < physicalDeviceMemoryProperties.memoryTypeCount; memoryTypeIndex++) {
			if ((memoryRequirements.memoryTypeBits & (1 << memoryTypeIndex)) && (physicalDeviceMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
				found = true;

				break;
			}
		}

		if (!found) {
			throw std::runtime_error("Failed to find suitable memory type for the buffer.");
		}

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		if (vkAllocateMemory(object, &memoryAllocateInfo, nullptr, &memory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate memory for the buffer.");
		}
	}

	void VulkanDevice::writeBuffer(const uint64 size, const void* data) const {
		PROFILE_SCOPE(FUNC_SIG);

		void* destination;

		vkMapMemory(object, memory, 0, size, 0, &destination);

		memcpy(destination, data, static_cast<std::size_t>(size));

		vkUnmapMemory(object, memory);
	}

	void VulkanDevice::waitForFences() const {
		PROFILE_SCOPE(FUNC_SIG);

		// Note: UINT64_MAX disables the timeout.
		vkWaitForFences(object, 1, &flightFence->object, VK_TRUE, UINT64_MAX);
	}

	void VulkanDevice::resetFences() const {
		PROFILE_SCOPE(FUNC_SIG);

		vkResetFences(object, 1, &flightFence->object);
	}

	uint32 VulkanDevice::acquireNextImage(const ref<Swapchain>& swapchain) const {
		PROFILE_SCOPE(FUNC_SIG);

		uint32 imageIndex;

		const VkResult result = vkAcquireNextImageKHR(object, reinterpret_cast<const VulkanSwapchain*>(swapchain.get())->object, UINT64_MAX, acquireNextImageSemaphore->object, VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			throw std::runtime_error("TODO: Recreate swapchain.");
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire next image from the Vulkan swapchain.");
		}

		return imageIndex;
	}

	void VulkanDevice::submit(const ref<CommandBuffer>& commandBuffer) const {
		PROFILE_SCOPE(FUNC_SIG);

		const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &acquireNextImageSemaphore->object;
		submitInfo.pWaitDstStageMask = &waitDstStageMask;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &reinterpret_cast<const VulkanCommandBuffer*>(commandBuffer.get())->object;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore->object;

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, flightFence->object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to submit Vulkan graphics queue.");
		}
	}

	void VulkanDevice::present(const ref<Swapchain>& swapchain, const uint32 imageIndex) const {
		PROFILE_SCOPE(FUNC_SIG);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &signalSemaphore->object;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &reinterpret_cast<const VulkanSwapchain*>(swapchain.get())->object;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		const VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
			throw std::runtime_error("TODO: Framebuffer has been resized.");
		}

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present Vulkan swapchain image.");
		}
	}

	void VulkanDevice::waitIdle() const {
		vkDeviceWaitIdle(object);
	}
}