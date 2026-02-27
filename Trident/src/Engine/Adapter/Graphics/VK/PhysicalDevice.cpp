// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/PhysicalDevice.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	static uint32 calculateGraphicsQueueFamilyScore(const VkQueueFamilyProperties& queueFamilyProperties) {
		uint32 score = 0;

		if (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			score += 1;
		}

		return score;
	}

	static uint32 calculateComputeQueueFamilyScore(const VkQueueFamilyProperties& queueFamilyProperties) {
		uint32 score = 0;

		if (queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			score += 1;
		}

		return score;
	}

	static uint32 calculateTransferQueueFamilyScore(const VkQueueFamilyProperties& queueFamilyProperties) {
		uint32 score = 0;

		if (queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
			score += 1;

			if (!(queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT || queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
				// Prefer queues which don't support graphics or compute, so that transfers are non-blocking regarding other operations.
				score += 1;
			}
		}
		else if (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT || queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			// Graphics and compute queues always support transfer operations.
			score += 1;
		}

		return score;
	}

	static view<char8> formatPhysicalDeviceType(VkPhysicalDeviceType physicalDeviceType) {
		switch (physicalDeviceType) {
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "(integrated GPU)";
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return "(discrete GPU)";
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return "(virtual GPU)";
			case VK_PHYSICAL_DEVICE_TYPE_CPU: return "(CPU)";
			default: return "(unknown type)";
		}
	}

	void VK::PhysicalDevice::enumerateQueueFamilyProperties(const sequence<VkQueueFamilyProperties>& queueFamilies) {
		for (uint32 queueFamilyIndex = 0; queueFamilyIndex < queueFamilies.count(); queueFamilyIndex += 1) {
			const VkQueueFamilyProperties& queueFamily = queueFamilies[queueFamilyIndex];

			ADAPTER_INFO(
				"Properties for queue family []:\n"
				"- Queue count: []\n"
				"- Queue flags: []",
				queueFamilyIndex,
				queueFamily.queueCount,
				string_VkQueueFlags(queueFamily.queueFlags)
			);
		}
	}

	VK::PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice) :
		Trident::PhysicalDevice(physicalDevice)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		const view<char8> physicalDeviceType = formatPhysicalDeviceType(properties.deviceType);

		ADAPTER_INFO("Using [] [].", properties.deviceName, physicalDeviceType);

		selectQueueFamilyIndices();

		uint32 extensionPropertyCount = 0;
		VkResult result;

		result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionPropertyCount, nullptr);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to find the number of device extension properties ([]).", string_VkResult(result));

		sequence<VkExtensionProperties> extensionProperties(extensionPropertyCount);

		result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionPropertyCount, &extensionProperties[0]);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to enumerate device extension properties ([]).", string_VkResult(result));

		enabledExtensionNames.resize(extensionPropertyCount);

		#if defined(DEBUG_ENUMERATE_DEVICE_EXTENSIONS)
			ADAPTER_INFO("Enabled device extensions:");

			for (uint32 extensionPropertiesIndex = 0; extensionPropertiesIndex < extensionProperties.count(); extensionPropertiesIndex += 1) {
				ADAPTER_INFO("- [] (spec version: [])", extensionProperties[extensionPropertiesIndex].extensionName, extensionProperties[extensionPropertiesIndex].specVersion);

				enabledExtensionNames[extensionPropertiesIndex] = extensionProperties[extensionPropertiesIndex].extensionName;
			}
		#endif

		vkGetPhysicalDeviceFeatures(physicalDevice, &features);
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

		setLimits();
	}

	void VK::PhysicalDevice::setLimits() {
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
		limits.maxImageDimension1D = properties.limits.maxImageDimension1D;
		limits.maxImageDimension2D = properties.limits.maxImageDimension2D;
		limits.maxImageDimension3D = properties.limits.maxImageDimension3D;
		limits.maxImageLayerCount = properties.limits.maxImageArrayLayers;
		limits.maxMemoryAllocationCount = properties.limits.maxMemoryAllocationCount;
		limits.maxVertexInputAttributeCount = properties.limits.maxVertexInputAttributes;
		limits.maxVertexInputAttributeOffset = properties.limits.maxVertexInputAttributeOffset;
		limits.maxVertexInputBindingCount = properties.limits.maxVertexInputBindings;
		limits.maxVertexInputBindingStride = properties.limits.maxVertexInputBindingStride;
		limits.pointSizeGranularity = properties.limits.pointSizeGranularity;
		limits.minStorageBufferOffsetAlignment = properties.limits.minStorageBufferOffsetAlignment;
		limits.minUniformBufferOffsetAlignment = properties.limits.minUniformBufferOffsetAlignment;
	}

	void VK::PhysicalDevice::selectQueueFamilyIndices() {
		uint32 queueFamilyCount;

		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		ADAPTER_ASSERT(queueFamilyCount > 0, "Failed to find queues for this physical device.");

		sequence<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, &queueFamilies[0]);

		#if defined(BUILD_MODE_DEBUG)
			enumerateQueueFamilyProperties(queueFamilies);
		#endif

		uint32 maxGraphicsQueueFamilyScore = 0;
		uint32 maxComputeQueueFamilyScore = 0;
		uint32 maxTransferQueueFamilyScore = 0;

		for (uint32 queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex += 1) {
			const VkQueueFamilyProperties& queueFamily = queueFamilies[queueFamilyIndex];

			ADAPTER_ASSERT(queueFamily.queueCount > 0, "Queue family [] has no queues.", queueFamilyIndex);

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

		ADAPTER_ASSERT(maxGraphicsQueueFamilyScore > 0, "Failed to find a graphics queue family.");
		ADAPTER_ASSERT(maxComputeQueueFamilyScore > 0, "Failed to find a compute queue family.");
		ADAPTER_ASSERT(maxTransferQueueFamilyScore > 0, "Failed to find a transfer queue family.");
	}

	VkPhysicalDevice VK::Instance::selectPhysicalDevice() const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		uint32 physicalDeviceCount;
		VkResult result;

		result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to find the number of physical devices ([]).", string_VkResult(result));
		DEBUG_ASSERT(physicalDeviceCount > 0);

		sequence<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

		result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, &physicalDevices[0]);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to enumerate physical devices ([]).", string_VkResult(result));

		VkPhysicalDeviceProperties physicalDeviceProperties;
		uint32 maxPhysicalDeviceScore = 0;
		uint32 bestPhysicalDeviceIndex = 0;

		#if defined(DEBUG_ENUMERATE_PHYSICAL_DEVICES)
			std::string physicalDeviceType;
		#endif

		#if defined(DEBUG_ENUMERATE_PHYSICAL_DEVICES)
			ADAPTER_INFO("Available physical devices:");
		#endif

		for (uint32 physicalDeviceIndex = 0; physicalDeviceIndex < physicalDevices.count(); physicalDeviceIndex += 1) {
			const VkPhysicalDevice physicalDevice = physicalDevices[physicalDeviceIndex];

			vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

			const uint32 physicalDeviceScore = calculatePhysicalDeviceScore(physicalDeviceProperties);

			if (physicalDeviceScore >= maxPhysicalDeviceScore) {
				maxPhysicalDeviceScore = physicalDeviceScore;
				bestPhysicalDeviceIndex = physicalDeviceIndex;
			}

			#if defined(DEBUG_ENUMERATE_PHYSICAL_DEVICES)
				switch (physicalDeviceProperties.deviceType) {
					case VK_PHYSICAL_DEVICE_TYPE_OTHER:
						physicalDeviceType = "unknown type";

						break;
					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
						physicalDeviceType = "integrated GPU";

						break;
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
						physicalDeviceType = "discrete GPU";

						break;
					case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
						physicalDeviceType = "virtual GPU";

						break;
					case VK_PHYSICAL_DEVICE_TYPE_CPU:
						physicalDeviceType = "CPU";

						break;
				}

				ADAPTER_INFO("- [] ([])", physicalDeviceProperties.deviceName, physicalDeviceType);
			#endif
		}

		ADAPTER_ASSERT(maxPhysicalDeviceScore > 0, "Could not select a suitable physical device.");

		return physicalDevices[bestPhysicalDeviceIndex];
	}

	uint32 VK::Instance::calculatePhysicalDeviceScore(const VkPhysicalDeviceProperties& physicalDeviceProperties) const {
		if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			return 1 + !preferIntegratedGpu;
		}

		if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
			return 1 + preferIntegratedGpu;
		}

		return 0;
	}
}