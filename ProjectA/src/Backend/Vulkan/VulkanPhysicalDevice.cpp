#include "pch.hpp"

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

namespace ProjectA {
	VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance) {
		PROFILE_SCOPE(FUNC_SIG);

		uint32 physicalDeviceCount;
		VkResult result;

		result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to find the number of Vulkan physical devices ([]).", string_VkResult(result));

		DEBUG_ASSERT(physicalDeviceCount > 0);

		fchain<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

		result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, &physicalDevices[0]);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to enumerate Vulkan physical devices ([]).", string_VkResult(result));

		uint32 bestPhysicalDeviceIndex = 0;
		uint32 bestPhysicalDeviceScore = 0;

		for (uint32 physicalDeviceIndex = 0; physicalDeviceIndex < physicalDevices.count; physicalDeviceIndex += 1) {
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

		fchain<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);

		vkGetPhysicalDeviceQueueFamilyProperties(object, &queueFamilyPropertyCount, &queueFamilyProperties[0]);

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

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to find the number of Vulkan device extension properties ([]).", string_VkResult(result));

		fchain<VkExtensionProperties> extensionProperties(extensionPropertyCount);

		result = vkEnumerateDeviceExtensionProperties(object, nullptr, &extensionPropertyCount, &extensionProperties[0]);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to enumerate Vulkan device extension properties ([]).", string_VkResult(result));

		enabledExtensionNames.resize(extensionPropertyCount);

		ENGINE_TRACE("Enabled device extensions:");

		for (uint32 extensionPropertiesIndex = 0; extensionPropertiesIndex < extensionProperties.count; extensionPropertiesIndex += 1) {
			ENGINE_TRACE("- [] (spec version: [])", extensionProperties[extensionPropertiesIndex].extensionName, extensionProperties[extensionPropertiesIndex].specVersion);

			enabledExtensionNames[extensionPropertiesIndex] = extensionProperties[extensionPropertiesIndex].extensionName;
		}

		vkGetPhysicalDeviceFeatures(object, &features);
		vkGetPhysicalDeviceMemoryProperties(object, &memoryProperties);
		vkGetPhysicalDeviceProperties(object, &properties);

		std::string deviceType;

		switch (properties.deviceType) {
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				deviceType = " (integrated GPU)";

				break;
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				deviceType = " (discrete GPU)";

				break;
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				deviceType = " (virtual GPU)";

				break;
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				deviceType = " (CPU)";

				break;
		}

		ENGINE_INFO("Using [][].", properties.deviceName, deviceType);

		setLimits();
	}

	uint32 VulkanPhysicalDevice::calculateScore(VkPhysicalDevice physicalDevice) {
		// TODO: Implement score system.
		return 1;
	}

	void VulkanPhysicalDevice::setLimits() {
		limits.bufferImageGranularity = properties.limits.bufferImageGranularity;
		limits.lineWidthGranularity = properties.limits.lineWidthGranularity;
		limits.maxBoundDescriptorSetCount = properties.limits.maxBoundDescriptorSets;
		limits.maxColorAttachmentCount = properties.limits.maxColorAttachments;
		limits.maxComputeWorkgroupCount[0] = properties.limits.maxComputeWorkGroupCount[0];
		limits.maxComputeWorkgroupCount[1] = properties.limits.maxComputeWorkGroupCount[1];
		limits.maxComputeWorkgroupCount[2] = properties.limits.maxComputeWorkGroupCount[2];
		limits.maxComputeWorkgroupSize[0] = properties.limits.maxComputeWorkGroupSize[0];
		limits.maxComputeWorkgroupSize[1] = properties.limits.maxComputeWorkGroupSize[1];
		limits.maxComputeWorkgroupSize[2] = properties.limits.maxComputeWorkGroupSize[2];
		limits.maxFramebufferSize[0] = properties.limits.maxFramebufferWidth;
		limits.maxFramebufferSize[1] = properties.limits.maxFramebufferHeight;
		limits.maxFramebufferLayerCount = properties.limits.maxFramebufferLayers;
		limits.maxImageArrayLayerCount = properties.limits.maxImageArrayLayers;
		limits.maxMemoryAllocationCount = properties.limits.maxMemoryAllocationCount;
		limits.maxVertexInputAttributeCount = properties.limits.maxVertexInputAttributes;
		limits.maxVertexInputAttributeOffset = properties.limits.maxVertexInputAttributeOffset;
		limits.maxVertexInputBindingCount = properties.limits.maxVertexInputBindings;
		limits.maxVertexInputBindingStride = properties.limits.maxVertexInputBindingStride;
		limits.pointSizeGranularity = properties.limits.pointSizeGranularity;
	}
}