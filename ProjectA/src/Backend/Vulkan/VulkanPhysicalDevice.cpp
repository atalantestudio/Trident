#include "pch.hpp"

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

namespace ProjectA {
	VulkanPhysicalDevice::VulkanPhysicalDevice(const VkInstance instance) {
		PROFILE_SCOPE(FUNC_SIG);

		uint32 physicalDeviceCount;

		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

		if (physicalDeviceCount == 0) {
			throw std::runtime_error("Could not find physical device(s).");
		}

		chain<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

		uint32 bestPhysicalDeviceIndex = 0;
		uint32 bestPhysicalDeviceScore = 0;

		for (uint32 index = 0; index < physicalDevices.size(); index += 1) {
			const uint32 score = calculateScore(physicalDevices[index]);

			if (score >= bestPhysicalDeviceScore) {
				bestPhysicalDeviceIndex = index;
				bestPhysicalDeviceScore = score;
			}
		}

		// Check the score of the best candidate.
		if (bestPhysicalDeviceScore == 0) {
			throw std::runtime_error("Could not select a suitable physical device.");
		}

		object = physicalDevices[bestPhysicalDeviceIndex];

		uint32 queueFamilyPropertyCount;

		vkGetPhysicalDeviceQueueFamilyProperties(object, &queueFamilyPropertyCount, nullptr);

		if (queueFamilyPropertyCount == 0) {
			throw std::runtime_error("Could not find queues for this physical device.");
		}

		chain<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);

		vkGetPhysicalDeviceQueueFamilyProperties(object, &queueFamilyPropertyCount, queueFamilyProperties.data());

		// Find a queue family that handles graphics.
		for (uint32 index = 0; index < queueFamilyProperties.size(); index += 1) {
			if (queueFamilyProperties[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				this->graphicsQueueFamilyIndex = index;

				break;
			}
		}

		uint32 enabledExtensionCount = 0;

		vkEnumerateDeviceExtensionProperties(object, nullptr, &enabledExtensionCount, nullptr);

		chain<VkExtensionProperties> extensionProperties(enabledExtensionCount);

		vkEnumerateDeviceExtensionProperties(object, nullptr, &enabledExtensionCount, extensionProperties.data());

		enabledExtensionNames.resize(enabledExtensionCount);

		//ENGINE_TRACE("Device extensions:");

		for (uint32 i = 0; i < extensionProperties.size(); i += 1) {
			//ENGINE_TRACE("- [] (spec version: [])", extensionProperties[i].extensionName, extensionProperties[i].specVersion);

			enabledExtensionNames[i] = extensionProperties[i].extensionName;
		}

		vkGetPhysicalDeviceFeatures(object, &features);
	}

	uint32 VulkanPhysicalDevice::calculateScore(const VkPhysicalDevice physicalDevice) {
		// TODO: Implement score system.
		return 1;
	}
}