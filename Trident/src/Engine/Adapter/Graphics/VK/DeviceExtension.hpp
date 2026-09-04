// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/DeviceExtensionName.hpp"

namespace Trident::VK {
	inline void enableDeviceExtension(DeviceExtensionName extension, std::vector<const char8*>& enabledExtensionNames, VkPhysicalDeviceVulkan11Features& vulkan11Features, void** nextFeature) {
		switch (extension) {
			case EXT_hardware_ray_tracing: {
				enabledExtensionNames.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
				enabledExtensionNames.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
				enabledExtensionNames.emplace_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);

				VkPhysicalDeviceVulkan12Features* vulkan12Features = static_cast<VkPhysicalDeviceVulkan12Features*>(vulkan11Features.pNext);

				vulkan12Features->bufferDeviceAddress = true;

				static VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};
				accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
				accelerationStructureFeatures.pNext = nullptr;
				accelerationStructureFeatures.accelerationStructure = true;

				static VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{};
				rayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
				rayTracingPipelineFeatures.pNext = &accelerationStructureFeatures;
				rayTracingPipelineFeatures.rayTracingPipeline = true;

				*nextFeature = &rayTracingPipelineFeatures;
				nextFeature = &rayTracingPipelineFeatures.pNext;

				break;
			}
		}
	}
}