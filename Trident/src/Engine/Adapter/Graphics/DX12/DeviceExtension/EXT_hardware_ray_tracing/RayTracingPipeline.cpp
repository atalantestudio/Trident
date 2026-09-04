// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/RayTracingPipeline.hpp"

namespace Trident::DX12 {
	void destroyRayTracingPipeline(const Trident::Device* device, const Trident::RayTracingPipeline* rayTracingPipeline) {
		const DX12::Device* dxDevice = static_cast<const DX12::Device*>(device);
		const DX12::RayTracingPipeline* dxRayTracingPipeline = static_cast<const DX12::RayTracingPipeline*>(rayTracingPipeline);

		// TODO(graphics/dx12/ext_hardware_ray_tracing/ray-tracing-pipeline)
	}

	void getRayTracingShaderGroupHandles(const Trident::Device* device, const Trident::RayTracingPipeline* pipeline, range<uint32, uint32> groupRange, void* mappedRange) {
		// TODO(graphics/dx12/ext_hardware_ray_tracing/ray-tracing-pipeline)
	}
}