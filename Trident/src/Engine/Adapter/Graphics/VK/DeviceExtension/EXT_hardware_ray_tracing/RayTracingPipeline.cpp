// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/RayTracingPipeline.hpp"
#include "Engine/API/Graphics/PhysicalDevice.hpp"

namespace Trident::VK {
	void destroyRayTracingPipeline(const Trident::Device* device, const Trident::RayTracingPipeline* rayTracingPipeline) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);
		const VK::RayTracingPipeline* vkPipeline = static_cast<const VK::RayTracingPipeline*>(rayTracingPipeline);

		vkDevice->vkDestroyPipeline(vkDevice->device, vkPipeline->pipeline, nullptr);
	}

	void getRayTracingShaderGroupHandles(const Trident::Device* device, const Trident::RayTracingPipeline* pipeline, range<uint32, uint32> groupRange, void* mappedRange) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);
		const VK::RayTracingPipeline* vkPipeline = static_cast<const VK::RayTracingPipeline*>(pipeline);
		const uint32 shaderGroupHandleSize = device->getPhysicalDevice()->getLimits().shaderGroupHandleSize;

		const VkResult result = vkDevice->vkGetRayTracingShaderGroupHandlesKHR(vkDevice->device, vkPipeline->pipeline, groupRange.offset, groupRange.count, shaderGroupHandleSize * groupRange.count, mappedRange);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get ray tracing shader group handles.");
	}
}