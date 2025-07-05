#include "pch.hpp"

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

namespace ProjectA {
	static std::string formatPhysicalDeviceType(VkPhysicalDeviceType physicalDeviceType) {
		switch (physicalDeviceType) {
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				return "(integrated GPU)";
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				return "(discrete GPU)";
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				return "(virtual GPU)";
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				return "(CPU)";
			default:
				return "(unknown type)";
		}
	}

	VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance) {
		PROFILE_SCOPE(FUNC_SIG);

		uint32 physicalDeviceCount;
		VkResult result;

		// TODO: Call vkEnumeratePhysicalDevices once.
		result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to find the number of Vulkan physical devices ([]).", string_VkResult(result));

		DEBUG_ASSERT(physicalDeviceCount > 0);

		sequence<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

		result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, &physicalDevices[0]);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to enumerate Vulkan physical devices ([]).", string_VkResult(result));

		uint32 maxPhysicalDeviceScore = 0;
		uint32 bestPhysicalDeviceIndex = 0;

		for (uint32 physicalDeviceIndex = 0; physicalDeviceIndex < physicalDevices.count; physicalDeviceIndex += 1) {
			const VkPhysicalDevice physicalDevice = physicalDevices[physicalDeviceIndex];

			vkGetPhysicalDeviceProperties(physicalDevice, &properties);

			const uint32 physicalDeviceScore = calculatePhysicalDeviceScore(properties);

			if (physicalDeviceScore >= maxPhysicalDeviceScore) {
				maxPhysicalDeviceScore = physicalDeviceScore;
				bestPhysicalDeviceIndex = physicalDeviceIndex;
			}
		}

		ENGINE_ASSERT(maxPhysicalDeviceScore > 0, "Could not select a suitable physical device.");

		object = physicalDevices[bestPhysicalDeviceIndex];

		vkGetPhysicalDeviceProperties(object, &properties);

		const std::string physicalDeviceType = formatPhysicalDeviceType(properties.deviceType);

		ENGINE_INFO("Using [] [].", properties.deviceName, physicalDeviceType);

		selectQueueFamilyIndices();

		uint32 extensionPropertyCount = 0;

		result = vkEnumerateDeviceExtensionProperties(object, nullptr, &extensionPropertyCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to find the number of Vulkan device extension properties ([]).", string_VkResult(result));

		sequence<VkExtensionProperties> extensionProperties(extensionPropertyCount);

		result = vkEnumerateDeviceExtensionProperties(object, nullptr, &extensionPropertyCount, &extensionProperties[0]);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to enumerate Vulkan device extension properties ([]).", string_VkResult(result));

		enabledExtensionNames.resize(extensionPropertyCount);

		ENGINE_INFO("Enabled device extensions:");

		/* for (uint32 extensionPropertiesIndex = 0; extensionPropertiesIndex < extensionProperties.count; extensionPropertiesIndex += 1) {
			ENGINE_INFO("- [] (spec version: [])", extensionProperties[extensionPropertiesIndex].extensionName, extensionProperties[extensionPropertiesIndex].specVersion);

			enabledExtensionNames[extensionPropertiesIndex] = extensionProperties[extensionPropertiesIndex].extensionName;
		} */

		vkGetPhysicalDeviceFeatures(object, &features);
		vkGetPhysicalDeviceMemoryProperties(object, &memoryProperties);

		setLimits();
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
		limits.maxImageLayerCount = properties.limits.maxImageArrayLayers;
		limits.maxMemoryAllocationCount = properties.limits.maxMemoryAllocationCount;
		limits.maxVertexInputAttributeCount = properties.limits.maxVertexInputAttributes;
		limits.maxVertexInputAttributeOffset = properties.limits.maxVertexInputAttributeOffset;
		limits.maxVertexInputBindingCount = properties.limits.maxVertexInputBindings;
		limits.maxVertexInputBindingStride = properties.limits.maxVertexInputBindingStride;
		limits.pointSizeGranularity = properties.limits.pointSizeGranularity;
	}

	void VulkanPhysicalDevice::selectQueueFamilyIndices() {
		uint32 queueFamilyCount;

		vkGetPhysicalDeviceQueueFamilyProperties(object, &queueFamilyCount, nullptr);

		ENGINE_ASSERT(queueFamilyCount > 0, "Could not find queues for this physical device.");

		sequence<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

		vkGetPhysicalDeviceQueueFamilyProperties(object, &queueFamilyCount, &queueFamilies[0]);

		#ifdef BUILD_MODE_DEBUG
			enumerateQueueFamilyProperties(queueFamilies);
		#endif

		uint32 maxGraphicsQueueFamilyScore = 0;
		uint32 maxComputeQueueFamilyScore = 0;
		uint32 maxTransferQueueFamilyScore = 0;

		for (uint32 queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex += 1) {
			const VkQueueFamilyProperties& queueFamily = queueFamilies[queueFamilyIndex];

			ENGINE_ASSERT(queueFamily.queueCount > 0, "Queue family [] has no queues.", queueFamilyIndex);

			const uint32 graphicsQueueFamilyScore = calculateGraphicsQueueFamilyScore(queueFamily);

			if (graphicsQueueFamilyScore > maxGraphicsQueueFamilyScore) {
				maxGraphicsQueueFamilyScore = graphicsQueueFamilyScore;
				graphicsQueueFamilyIndex = queueFamilyIndex;
			}

			const uint32 computeQueueFamilyScore = calculateComputeQueueFamilyScore(queueFamily);

			if (computeQueueFamilyScore > maxComputeQueueFamilyScore) {
				maxComputeQueueFamilyScore = computeQueueFamilyScore;
				computeQueueFamilyIndex = queueFamilyIndex;
			}

			const uint32 transferQueueFamilyScore = calculateTransferQueueFamilyScore(queueFamily);

			if (transferQueueFamilyScore > maxTransferQueueFamilyScore) {
				maxTransferQueueFamilyScore = transferQueueFamilyScore;
				transferQueueFamilyIndex = queueFamilyIndex;
			}
		}

		ENGINE_ASSERT(maxGraphicsQueueFamilyScore > 0, "Failed to find a Vulkan graphics queue family.");
		ENGINE_ASSERT(maxComputeQueueFamilyScore > 0, "Failed to find a Vulkan compute queue family.");
		ENGINE_ASSERT(maxTransferQueueFamilyScore > 0, "Failed to find a Vulkan transfer queue family.");
	}

	void VulkanPhysicalDevice::enumerateQueueFamilyProperties(const sequence<VkQueueFamilyProperties>& queueFamilies) {
		for (uint32 queueFamilyIndex = 0; queueFamilyIndex < queueFamilies.count; queueFamilyIndex += 1) {
			const VkQueueFamilyProperties& queueFamily = queueFamilies[queueFamilyIndex];

			ENGINE_INFO(
				"Properties for queue family []:\n"
				"- Queue count: []\n"
				"- Queue flags: []",
				queueFamilyIndex,
				queueFamily.queueCount,
				string_VkQueueFlags(queueFamily.queueFlags)
			);
		}
	}

	uint32 VulkanPhysicalDevice::calculatePhysicalDeviceScore(const VkPhysicalDeviceProperties& physicalDeviceProperties) {
		#if PREFERRED_GPU_TYPE == GPU_TYPE_DISCRETE
			if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				return 1;
			}
		#endif

		#if PREFERRED_GPU_TYPE == GPU_TYPE_INTEGRATED
			if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
				return 1;
			}
		#endif

		return 0;
	}

	uint32 VulkanPhysicalDevice::calculateGraphicsQueueFamilyScore(const VkQueueFamilyProperties& queueFamilyProperties) {
		if (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			return 1;
		}

		return 0;
	}

	uint32 VulkanPhysicalDevice::calculateComputeQueueFamilyScore(const VkQueueFamilyProperties& queueFamilyProperties) {
		if (queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			return 1;
		}

		return 0;
	}

	uint32 VulkanPhysicalDevice::calculateTransferQueueFamilyScore(const VkQueueFamilyProperties& queueFamilyProperties) {
		if (queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
			// Prefer queues which don't support graphics or compute, so that transfers are non-blocking regarding other operations.
			if (!(queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && !(queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
				return 3;
			}

			return 2;
		}

		if (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT || queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			return 1;
		}

		return 1;
	}
}