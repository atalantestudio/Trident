// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/RayTracingPipeline.hpp"

namespace Trident::VK {
	void destroyRayTracingPipeline(const Trident::Device* device, const Trident::RayTracingPipeline* rayTracingPipeline) {
		const VK::Device* vkDevice = static_cast<const VK::Device*>(device);
		const VK::RayTracingPipeline* vkRayTracingPipeline = static_cast<const VK::RayTracingPipeline*>(rayTracingPipeline);

		// TODO(graphics/vk/ext_hardware_ray_tracing/ray-tracing-pipeline)
		// vkDevice->vkDestroyRayTracingPipelineKHR(vkDevice->device, vkRayTracingPipeline->rayTracingPipeline, nullptr);
	}
}