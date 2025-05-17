#include "pch.hpp"

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

namespace ProjectA {
	VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance) {
		PROFILE_SCOPE(FUNC_SIG);

		uint32 physicalDeviceCount;
		VkResult result;

		result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to find the number of Vulkan physical devices (code: []).", result);
		ENGINE_ASSERT(physicalDeviceCount > 0, "Could not find any physical device.");

		chain<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

		result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to enumerate Vulkan physical devices (code: []).", result);

		uint32 bestPhysicalDeviceIndex = 0;
		uint32 bestPhysicalDeviceScore = 0;

		for (uint32 physicalDeviceIndex = 0; physicalDeviceIndex < physicalDevices.size(); physicalDeviceIndex += 1) {
			const uint32 score = calculateScore(physicalDevices[physicalDeviceIndex]);

			if (score >= bestPhysicalDeviceScore) {
				bestPhysicalDeviceIndex = physicalDeviceIndex;
				bestPhysicalDeviceScore = score;
			}
		}

		ENGINE_ASSERT(bestPhysicalDeviceScore > 0, "Could not select a suitable physical device.");

		object = physicalDevices[bestPhysicalDeviceIndex];

		uint32 queueFamilyPropertyCount;

		vkGetPhysicalDeviceQueueFamilyProperties(object, &queueFamilyPropertyCount, nullptr);

		ENGINE_ASSERT(queueFamilyPropertyCount > 0, "Could not find queues for this physical device.");

		chain<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);

		vkGetPhysicalDeviceQueueFamilyProperties(object, &queueFamilyPropertyCount, queueFamilyProperties.data());

		for (uint32 queueFamilyPropertyIndex = 0; queueFamilyPropertyIndex < queueFamilyPropertyCount; queueFamilyPropertyIndex += 1) {
			if (queueFamilyProperties[queueFamilyPropertyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				graphicsQueueFamilyIndex = queueFamilyPropertyIndex;
			}

			if (queueFamilyProperties[queueFamilyPropertyIndex].queueFlags & VK_QUEUE_COMPUTE_BIT) {
				computeQueueFamilyIndex = queueFamilyPropertyIndex;
			}
		}

		ENGINE_ASSERT(graphicsQueueFamilyIndex != std::numeric_limits<uint32>::max(), "Failed to find Vulkan graphics queue index.");
		ENGINE_ASSERT(computeQueueFamilyIndex != std::numeric_limits<uint32>::max(), "Failed to find Vulkan compute queue index.");

		uint32 extensionPropertyCount = 0;

		result = vkEnumerateDeviceExtensionProperties(object, nullptr, &extensionPropertyCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to find the number of Vulkan device extension properties (code: []).", result);

		chain<VkExtensionProperties> extensionProperties(extensionPropertyCount);

		result = vkEnumerateDeviceExtensionProperties(object, nullptr, &extensionPropertyCount, extensionProperties.data());

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to enumerate Vulkan device extension properties (code: []).", result);

		enabledExtensionNames.resize(extensionPropertyCount);

		for (uint32 i = 0; i < extensionProperties.size(); i += 1) {
			//ENGINE_INFO("- [] (spec version: [])", extensionProperties[i].extensionName, extensionProperties[i].specVersion);

			enabledExtensionNames[i] = extensionProperties[i].extensionName;
		}

		vkGetPhysicalDeviceFeatures(object, &features);

		VkPhysicalDeviceProperties physicalDeviceProperties;

		vkGetPhysicalDeviceProperties(object, &physicalDeviceProperties);

		limits.maxComputeWorkgroupCount[0] = physicalDeviceProperties.limits.maxComputeWorkGroupCount[0];
		limits.maxComputeWorkgroupCount[1] = physicalDeviceProperties.limits.maxComputeWorkGroupCount[1];
		limits.maxComputeWorkgroupCount[2] = physicalDeviceProperties.limits.maxComputeWorkGroupCount[2];
	}

	const PhysicalDeviceLimits& VulkanPhysicalDevice::getLimits() const {
		return limits;
	}

	uint32 VulkanPhysicalDevice::calculateScore(VkPhysicalDevice physicalDevice) {
		// TODO: Implement score system.
		return 1;
	}
}