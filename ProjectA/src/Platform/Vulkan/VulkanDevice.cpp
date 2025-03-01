#include "pch.hpp"

#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	// TODO: Pass enabled extension names as parameter.
	const std::vector<const char*> VulkanDevice::enabledExtensionNames = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
	}

	VulkanDevice::~VulkanDevice() {
		PROFILE_SCOPE(FUNC_SIG);

		vkFreeMemory(object, memory, nullptr);
		vkDestroyDevice(object, nullptr);
	}

	void VulkanDevice::allocateMemory(const VkMemoryRequirements& memoryRequirements) {
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties{};

		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

		const VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		uint32 memoryTypeIndex;
		bool found = false;

		for (uint32 i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++) {
			if ((memoryRequirements.memoryTypeBits & (1 << i)) && (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
				memoryTypeIndex = i;
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
		void* destination;

		vkMapMemory(object, memory, 0, size, 0, &destination);

		memcpy(destination, data, static_cast<std::size_t>(size));

		vkUnmapMemory(object, memory);
	}
}