#include "pch.hpp"

#include "Backend/Vulkan/VulkanCommandBuffer.hpp"
#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanFence.hpp"
#include "Backend/Vulkan/VulkanSemaphore.hpp"
#include "Backend/Vulkan/VulkanShaderModule.hpp"
#include "Backend/Vulkan/VulkanSwapchain.hpp"

namespace ProjectA {
	// TODO: Client-side extension request.
	const std::vector<const char8*> VulkanDevice::enabledExtensionNames = {
		VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	// NOTE: ~3Mb in Release mode.
	VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface) :
		physicalDevice(createRef<VulkanPhysicalDevice>(physicalDevice))
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		std::vector<uint32> queueFamilyIndices = {
			physicalDevice.graphicsQueueFamilyIndex,
			physicalDevice.computeQueueFamilyIndex,
			physicalDevice.transferQueueFamilyIndex,
		};

		std::sort(queueFamilyIndices.begin(), queueFamilyIndices.end());

		queueFamilyIndices.erase(std::unique(queueFamilyIndices.begin(), queueFamilyIndices.end()), queueFamilyIndices.end());

		sequence<VkDeviceQueueCreateInfo> deviceQueueCreateInfos(queueFamilyIndices.size());

		static constexpr float32 DEFAULT_QUEUE_PRIORITY = 1.0f;

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

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan device ([]).", string_VkResult(result));

		queues[static_cast<uint32>(QueueFamilyCapability::GRAPHICS)] = createRef<VulkanQueue>(*this, physicalDevice.graphicsQueueFamilyIndex);
		queues[static_cast<uint32>(QueueFamilyCapability::COMPUTE)] = createRef<VulkanQueue>(*this, physicalDevice.computeQueueFamilyIndex);
		queues[static_cast<uint32>(QueueFamilyCapability::TRANSFER)] = createRef<VulkanQueue>(*this, physicalDevice.transferQueueFamilyIndex);

		vkGetPhysicalDeviceMemoryProperties(physicalDevice.object, &memoryProperties);
	}

	VulkanDevice::~VulkanDevice() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (const ref<VulkanQueue>& queue : queues) {
			// Destroy the queue's command pool.
			queue.~shared_ptr();
		}

		vkDestroyDevice(object, nullptr);
	}

	ref<PhysicalDevice> VulkanDevice::getPhysicalDevice() const {
		return physicalDevice;
	}

	ref<Queue> VulkanDevice::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return queues[static_cast<uint32>(queueFamilyCapability)];
	}

	void VulkanDevice::waitForFence(const ref<Fence>& fence) const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VkResult result = vkWaitForFences(
			object,
			1,
			&std::static_pointer_cast<VulkanFence>(fence)->object,
			VK_TRUE,
			max<uint64>()
		);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to wait for Vulkan fence ([]).", string_VkResult(result));
	}

	void VulkanDevice::resetFence(const ref<Fence>& fence) const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VkResult result = vkResetFences(
			object,
			1,
			&std::static_pointer_cast<VulkanFence>(fence)->object
		);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to reset Vulkan fence ([]).", string_VkResult(result));
	}

	AcquireNextImageStatus VulkanDevice::acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VkResult result = vkAcquireNextImageKHR(
			object,
			std::static_pointer_cast<VulkanSwapchain>(swapchain)->object,
			max<uint64>(),
			std::static_pointer_cast<VulkanSemaphore>(semaphore)->object,
			VK_NULL_HANDLE,
			&imageIndex
		);

		if (result == VK_SUCCESS) {
			return AcquireNextImageStatus::SUCCESS;
		}

		if (result == VK_SUBOPTIMAL_KHR) {
			return AcquireNextImageStatus::SUBOPTIMAL;
		}

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			return AcquireNextImageStatus::ERROR_OUT_OF_DATE;
		}

		DEBUG_ASSERT(false);

		return AcquireNextImageStatus::ERROR_UNHANDLED;
	}

	void VulkanDevice::waitIdle() const {
		const VkResult result = vkDeviceWaitIdle(object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to idle wait ([]).", string_VkResult(result));
	}
}