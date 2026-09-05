// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/RayTracingPipeline.hpp"

namespace Trident::DX12 {
	void destroyRayTracingPipeline(const Trident::Device* device, const Trident::RayTracingPipeline* pipeline) {
		const DX12::RayTracingPipeline* dxPipeline = static_cast<const DX12::RayTracingPipeline*>(pipeline);

		dxPipeline->stateObjectProperties->Release();
		dxPipeline->stateObject->Release();
	}

	void getRayTracingShaderGroupHandles(const Trident::Device* device, const Trident::RayTracingPipeline* pipeline, range<uint32, uint32> groupRange, void* mappedRange) {
		const PhysicalDeviceLimits& limits = device->getPhysicalDevice()->getLimits();
		const DX12::Device* dxDevice = static_cast<const DX12::Device*>(device);
		const DX12::RayTracingPipeline* dxPipeline = static_cast<const DX12::RayTracingPipeline*>(pipeline);

		PBYTE MappedRange = static_cast<PBYTE>(mappedRange);
		UINT MappedRangeOffset = 0;
		UINT ExportIndex = 0;

		while (ExportIndex < groupRange.count) {
			const WCHAR* ExportName = dxPipeline->exportNames[groupRange.offset + ExportIndex];
			const PBYTE ShaderIdentifier = static_cast<PBYTE>(dxPipeline->stateObjectProperties->GetShaderIdentifier(ExportName));

			std::copy_n(ShaderIdentifier, limits.shaderGroupHandleSize, &MappedRange[MappedRangeOffset]);

			MappedRangeOffset += limits.shaderGroupHandleSize;
			ExportIndex += 1;
		}
	}
}