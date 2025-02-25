#include "pch.hpp"

#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	VulkanDevice::VulkanDevice(const VkInstance& instance) : ObjectWrapper(nullptr) {
		PROFILE_SCOPE(FUNC_SIG);

		uint32 physicalDeviceCount;

		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

		if (physicalDeviceCount == 0) {
			throw std::runtime_error("Could not find physical device(s).");
		}

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

		uint32 bestPhysicalDeviceIndex = 0;
		int32 bestPhysicalDeviceScore = calculatePhysicalDeviceScore(physicalDevices[bestPhysicalDeviceIndex]);

		for (uint32 index = bestPhysicalDeviceIndex; index < physicalDevices.size(); index++) {
			const int32 score = calculatePhysicalDeviceScore(physicalDevices[index]);

			if (score > bestPhysicalDeviceScore) {
				bestPhysicalDeviceIndex = index;
				bestPhysicalDeviceScore = score;
			}
		}

		// Check the score of the best candidate.
		if (bestPhysicalDeviceScore == 0) {
			throw std::runtime_error("Could not select a suitable physical device.");
		}

		const VkPhysicalDevice& physicalDevice = physicalDevices[bestPhysicalDeviceIndex];

		const int32 queueFamilyIndex = findQueueFamilyIndex(physicalDevice);
		const float32 queuePriority = 1.0f;

		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

		// TODO: Specify enabled extensions
		deviceCreateInfo.enabledExtensionCount = 0;
		deviceCreateInfo.ppEnabledExtensionNames = nullptr;

		// TODO: Specify enabled features
		deviceCreateInfo.pEnabledFeatures = nullptr;

		if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create device.");
		}
	}

	VulkanDevice::~VulkanDevice() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyDevice(object, nullptr);
	}

	ShaderModule VulkanDevice::createShaderModule() const {
		return VulkanShaderModule(object, VK_SHADER_STAGE_VERTEX_BIT, "");
	}

	int32 VulkanDevice::calculatePhysicalDeviceScore(const VkPhysicalDevice& physicalDevice) {
		// TODO: Implement physical device score system.
		return 1;
	}

	int32 VulkanDevice::findQueueFamilyIndex(const VkPhysicalDevice& physicalDevice) {
		uint32 queueFamilyPropertyCount;

		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);

		if (queueFamilyPropertyCount == 0) {
			throw std::runtime_error("Could not find device queues.");
		}

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);

		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

		int32 graphicsQueueFamilyIndex = 0;

		// Find a queue family that handles graphics.
		for (int32 index = 0; index < queueFamilyProperties.size(); index++) {
			if (queueFamilyProperties[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				graphicsQueueFamilyIndex = index;

				break;
			}

			index++;
		}

		// TODO: Check surface support for found queue.

		return graphicsQueueFamilyIndex;
	}
}