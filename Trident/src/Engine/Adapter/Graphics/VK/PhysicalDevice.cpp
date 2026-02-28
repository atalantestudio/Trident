// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Instance.hpp"
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

			if (!(queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				score += 1;
			}
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
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: {
				return "(integrated GPU)";
			}
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: {
				return "(discrete GPU)";
			}
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: {
				return "(virtual GPU)";
			}
			case VK_PHYSICAL_DEVICE_TYPE_CPU: {
				return "(CPU)";
			}
			default: {
				return "(unknown type)";
			}
		}
	}

	static uint32 calculatePhysicalDeviceScore(const VkPhysicalDeviceProperties& physicalDeviceProperties, bool preferIntegratedGpu) {
		uint32 score = 0;

		if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && !preferIntegratedGpu) {
			score += 1;
		}

		if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && preferIntegratedGpu) {
			score += 1;
		}

		return score;
	}

	static void enumerateQueueFamilyProperties(view<VkQueueFamilyProperties> queueFamilies) {
		uint32 queueFamilyIndex = 0;

		while (queueFamilyIndex < queueFamilies.count()) {
			const VkQueueFamilyProperties& queueFamily = queueFamilies[queueFamilyIndex];

			ADAPTER_INFO(
				"Properties for queue family []:\n"
				"- Queue count: []\n"
				"- Queue flags: []",
				queueFamilyIndex,
				queueFamily.queueCount,
				string_VkQueueFlags(queueFamily.queueFlags)
			);

			queueFamilyIndex += 1;
		}
	}

	VK::PhysicalDevice::PhysicalDevice(const Trident::GraphicsAdapter* adapter) :
		Trident::PhysicalDevice(adapter)
	{}

	ref<Trident::PhysicalDevice> VK::selectPhysicalDevice(const Trident::Instance* instance, bool preferIntegratedGpu) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Instance* _instance = static_cast<const VK::Instance*>(instance);
		uint32 physicalDeviceCount;
		VkResult result;

		ref<VK::PhysicalDevice> _physicalDevice = createRef<VK::PhysicalDevice>(instance->adapter);
		_physicalDevice->instance = _instance;

		result = _instance->vkEnumeratePhysicalDevices(_instance->instance, &physicalDeviceCount, nullptr);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to find the number of physical devices ([]).", string_VkResult(result));
		DEBUG_ASSERT(physicalDeviceCount > 0);

		sequence<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

		result = _instance->vkEnumeratePhysicalDevices(_instance->instance, &physicalDeviceCount, &physicalDevices[0]);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to enumerate physical devices ([]).", string_VkResult(result));

		VkPhysicalDeviceProperties physicalDeviceProperties;
		uint32 maxPhysicalDeviceScore = 0;
		uint32 bestPhysicalDeviceIndex = 0;
		uint32 physicalDeviceIndex = 0;

		#if defined(DEBUG_ENUMERATE_PHYSICAL_DEVICES)
			ADAPTER_INFO("Available physical devices:");
		#endif

		while (physicalDeviceIndex < physicalDevices.count()) {
			const VkPhysicalDevice physicalDevice = physicalDevices[physicalDeviceIndex];

			_instance->vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

			const uint32 physicalDeviceScore = calculatePhysicalDeviceScore(physicalDeviceProperties, preferIntegratedGpu);

			if (physicalDeviceScore >= maxPhysicalDeviceScore) {
				maxPhysicalDeviceScore = physicalDeviceScore;
				bestPhysicalDeviceIndex = physicalDeviceIndex;
			}

			#if defined(DEBUG_ENUMERATE_PHYSICAL_DEVICES)
				const view<char8> physicalDeviceType = formatPhysicalDeviceType(physicalDeviceProperties.deviceType);

				ADAPTER_INFO("- [] []", physicalDeviceProperties.deviceName, physicalDeviceType);
			#endif

			physicalDeviceIndex += 1;
		}

		ADAPTER_ASSERT(maxPhysicalDeviceScore > 0, "Could not select a suitable physical device.");

		_physicalDevice->physicalDevice = physicalDevices[bestPhysicalDeviceIndex];

		_instance->vkGetPhysicalDeviceProperties(_physicalDevice->physicalDevice, &_physicalDevice->properties);

		const view<char8> physicalDeviceType = formatPhysicalDeviceType(_physicalDevice->properties.deviceType);

		ADAPTER_INFO("Using [] [].", _physicalDevice->properties.deviceName, physicalDeviceType);

		// Select queue family indices.
		{
			uint32 queueFamilyCount;

			_instance->vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice->physicalDevice, &queueFamilyCount, nullptr);

			ADAPTER_ASSERT(queueFamilyCount > 0, "Failed to find queues for this physical device.");

			sequence<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

			_instance->vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice->physicalDevice, &queueFamilyCount, &queueFamilies[0]);

			#if defined(BUILD_MODE_DEBUG)
				enumerateQueueFamilyProperties(queueFamilies);
			#endif

			uint32 maxGraphicsQueueFamilyScore = 0;
			uint32 maxComputeQueueFamilyScore = 0;
			uint32 maxTransferQueueFamilyScore = 0;
			uint32 queueFamilyIndex = 0;

			while (queueFamilyIndex < queueFamilyCount) {
				const VkQueueFamilyProperties& queueFamily = queueFamilies[queueFamilyIndex];

				ADAPTER_ASSERT(queueFamily.queueCount > 0, "Queue family [] has no queues.", queueFamilyIndex);

				const uint32 graphicsQueueFamilyScore = calculateGraphicsQueueFamilyScore(queueFamily);

				if (graphicsQueueFamilyScore > maxGraphicsQueueFamilyScore) {
					maxGraphicsQueueFamilyScore = graphicsQueueFamilyScore;
					_physicalDevice->graphicsQueueFamilyIndex = queueFamilyIndex;
				}

				const uint32 computeQueueFamilyScore = calculateComputeQueueFamilyScore(queueFamily);

				if (computeQueueFamilyScore > maxComputeQueueFamilyScore) {
					maxComputeQueueFamilyScore = computeQueueFamilyScore;
					_physicalDevice->computeQueueFamilyIndex = queueFamilyIndex;
				}

				const uint32 transferQueueFamilyScore = calculateTransferQueueFamilyScore(queueFamily);

				if (transferQueueFamilyScore > maxTransferQueueFamilyScore) {
					maxTransferQueueFamilyScore = transferQueueFamilyScore;
					_physicalDevice->transferQueueFamilyIndex = queueFamilyIndex;
				}

				queueFamilyIndex += 1;
			}

			ADAPTER_ASSERT(maxGraphicsQueueFamilyScore > 0, "Failed to find a graphics queue family.");
			ADAPTER_ASSERT(maxComputeQueueFamilyScore > 0, "Failed to find a compute queue family.");
			ADAPTER_ASSERT(maxTransferQueueFamilyScore > 0, "Failed to find a transfer queue family.");
		}

		uint32 extensionPropertyCount = 0;

		result = _instance->vkEnumerateDeviceExtensionProperties(_physicalDevice->physicalDevice, nullptr, &extensionPropertyCount, nullptr);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to find the number of device extension properties ([]).", string_VkResult(result));

		sequence<VkExtensionProperties> extensionProperties(extensionPropertyCount);

		result = _instance->vkEnumerateDeviceExtensionProperties(_physicalDevice->physicalDevice, nullptr, &extensionPropertyCount, &extensionProperties[0]);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to enumerate device extension properties ([]).", string_VkResult(result));

		#if defined(DEBUG_ENUMERATE_DEVICE_EXTENSIONS)
			ADAPTER_INFO("Enabled device extensions:");

			for (uint32 extensionPropertiesIndex = 0; extensionPropertiesIndex < extensionProperties.count(); extensionPropertiesIndex += 1) {
				ADAPTER_INFO("- [] (spec version: [])", extensionProperties[extensionPropertiesIndex].extensionName, extensionProperties[extensionPropertiesIndex].specVersion);
			}
		#endif

		_instance->vkGetPhysicalDeviceFeatures(_physicalDevice->physicalDevice, &_physicalDevice->features);
		_instance->vkGetPhysicalDeviceMemoryProperties(_physicalDevice->physicalDevice, &_physicalDevice->memoryProperties);

		// Query limits.
		{
			PhysicalDeviceLimits& limits = _physicalDevice->limits;
			limits.bufferImageGranularity = physicalDeviceProperties.limits.bufferImageGranularity;
			limits.lineWidthGranularity = physicalDeviceProperties.limits.lineWidthGranularity;
			limits.maxBoundDescriptorSetCount = physicalDeviceProperties.limits.maxBoundDescriptorSets;
			limits.maxColorAttachmentCount = physicalDeviceProperties.limits.maxColorAttachments;
			limits.maxComputeWorkgroupCount[0] = physicalDeviceProperties.limits.maxComputeWorkGroupCount[0];
			limits.maxComputeWorkgroupCount[1] = physicalDeviceProperties.limits.maxComputeWorkGroupCount[1];
			limits.maxComputeWorkgroupCount[2] = physicalDeviceProperties.limits.maxComputeWorkGroupCount[2];
			limits.maxComputeWorkgroupSize[0] = physicalDeviceProperties.limits.maxComputeWorkGroupSize[0];
			limits.maxComputeWorkgroupSize[1] = physicalDeviceProperties.limits.maxComputeWorkGroupSize[1];
			limits.maxComputeWorkgroupSize[2] = physicalDeviceProperties.limits.maxComputeWorkGroupSize[2];
			limits.maxFramebufferSize[0] = physicalDeviceProperties.limits.maxFramebufferWidth;
			limits.maxFramebufferSize[1] = physicalDeviceProperties.limits.maxFramebufferHeight;
			limits.maxFramebufferLayerCount = physicalDeviceProperties.limits.maxFramebufferLayers;
			limits.maxImageDimension1D = physicalDeviceProperties.limits.maxImageDimension1D;
			limits.maxImageDimension2D = physicalDeviceProperties.limits.maxImageDimension2D;
			limits.maxImageDimension3D = physicalDeviceProperties.limits.maxImageDimension3D;
			limits.maxImageLayerCount = physicalDeviceProperties.limits.maxImageArrayLayers;
			limits.maxMemoryAllocationCount = physicalDeviceProperties.limits.maxMemoryAllocationCount;
			limits.maxVertexInputAttributeCount = physicalDeviceProperties.limits.maxVertexInputAttributes;
			limits.maxVertexInputAttributeOffset = physicalDeviceProperties.limits.maxVertexInputAttributeOffset;
			limits.maxVertexInputBindingCount = physicalDeviceProperties.limits.maxVertexInputBindings;
			limits.maxVertexInputBindingStride = physicalDeviceProperties.limits.maxVertexInputBindingStride;
			limits.pointSizeGranularity = physicalDeviceProperties.limits.pointSizeGranularity;
			limits.minStorageBufferOffsetAlignment = physicalDeviceProperties.limits.minStorageBufferOffsetAlignment;
			limits.minUniformBufferOffsetAlignment = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
		}

		return _physicalDevice;
	}
}