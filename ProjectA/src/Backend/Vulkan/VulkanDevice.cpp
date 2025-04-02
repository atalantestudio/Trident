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
		VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface) :
		physicalDevice(createRef<VulkanPhysicalDevice>(physicalDevice))
	{
		PROFILE_SCOPE(FUNC_SIG);

		chain<uint32> queueFamilyIndices = {
			physicalDevice.graphicsQueueFamilyIndex,
			physicalDevice.computeQueueFamilyIndex,
			surface.presentQueueFamilyIndex,
		};

		std::sort(queueFamilyIndices.begin(), queueFamilyIndices.end());

		queueFamilyIndices.erase(std::unique(queueFamilyIndices.begin(), queueFamilyIndices.end()), queueFamilyIndices.end());

		fchain<VkDeviceQueueCreateInfo> deviceQueueCreateInfos(queueFamilyIndices.size());

		constexpr float32 DEFAULT_QUEUE_PRIORITY = 1.0f;

		for (uint32 queueFamilyIndexIndex = 0; queueFamilyIndexIndex < queueFamilyIndices.size(); queueFamilyIndexIndex += 1) {
			VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
			deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.pNext = nullptr;
			deviceQueueCreateInfo.flags = 0;
			deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndices[queueFamilyIndexIndex];
			deviceQueueCreateInfo.queueCount = 1;
			deviceQueueCreateInfo.pQueuePriorities = &DEFAULT_QUEUE_PRIORITY;

			deviceQueueCreateInfos[queueFamilyIndexIndex] = deviceQueueCreateInfo;
		}

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(deviceQueueCreateInfos.count);
		deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfos[0];
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32>(enabledExtensionNames.size());
		deviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();
		deviceCreateInfo.pEnabledFeatures = &physicalDevice.features;

		const VkResult result = vkCreateDevice(physicalDevice.object, &deviceCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan device (code: []).", result);

		computeQueue = createRef<VulkanQueue>(*this, physicalDevice.computeQueueFamilyIndex);
		graphicsQueue = createRef<VulkanQueue>(*this, physicalDevice.graphicsQueueFamilyIndex);
		presentQueue = createRef<VulkanQueue>(*this, surface.presentQueueFamilyIndex);

		vkGetPhysicalDeviceMemoryProperties(physicalDevice.object, &memoryProperties);
	}

	VulkanDevice::~VulkanDevice() {
		PROFILE_SCOPE(FUNC_SIG);

		computeQueue.~shared_ptr();
		graphicsQueue.~shared_ptr();
		presentQueue.~shared_ptr();

		vkDestroyDevice(object, nullptr);
	}

	uint32 VulkanDevice::findMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags) const {
		PROFILE_SCOPE(FUNC_SIG);

		for (uint32 memoryTypeIndex = 0; memoryTypeIndex < memoryProperties.memoryTypeCount; memoryTypeIndex += 1) {
			const VkMemoryType memoryType = memoryProperties.memoryTypes[memoryTypeIndex];
			const bool required = memoryRequirements.memoryTypeBits & BIT(memoryTypeIndex);

			if (required && (memoryType.propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
				return memoryTypeIndex;
			}
		}

		ENGINE_ASSERT(false, "Failed to find a suitable Vulkan memory type.");

		return 0;
	}

	ref<PhysicalDevice> VulkanDevice::getPhysicalDevice() const {
		return physicalDevice;
	}

	ref<Queue> VulkanDevice::getComputeQueue() const {
		return computeQueue;
	}

	ref<Queue> VulkanDevice::getGraphicsQueue() const {
		return graphicsQueue;
	}

	void VulkanDevice::waitForFence(const ref<Fence>& fence) const {
		PROFILE_SCOPE(FUNC_SIG);

		// UINT64_MAX disables the timeout.
		const VkResult result = vkWaitForFences(object, 1, &static_cast<const VulkanFence*>(fence.get())->object, VK_TRUE, UINT64_MAX);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to wait for Vulkan fences (code: []).", result);
	}

	void VulkanDevice::resetFence(const ref<Fence>& fence) const {
		PROFILE_SCOPE(FUNC_SIG);

		const VkResult result = vkResetFences(object, 1, &static_cast<const VulkanFence*>(fence.get())->object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to reset Vulkan fences (code: []).", result);
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

	Status VulkanDevice::present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, uint32 imageIndex) const {
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

		const VkResult result = vkQueuePresentKHR(presentQueue->object, &presentInfo);

		return getStatus(result);
	}

	void VulkanDevice::waitIdle() const {
		const VkResult result = vkDeviceWaitIdle(object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to idle wait (code: []).", result);
	}
}