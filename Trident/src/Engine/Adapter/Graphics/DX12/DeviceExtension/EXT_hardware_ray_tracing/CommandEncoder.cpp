// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/CommandEncoder.hpp"

namespace Trident::DX12 {
	void bindRayTracingPipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::RayTracingPipeline> pipeline) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void buildAccelerationStructures(Trident::CommandEncoder* commandEncoder, view<ref<Trident::AccelerationStructure>> accelerationStructures, view<BaseBufferView> accelerationStructureViews, view<BaseBufferView> scratchViews) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Device* dxDevice = static_cast<const DX12::Device*>(commandEncoder->device);
		const UINT AccelerationStructureCount = static_cast<UINT>(accelerationStructures.count());

		DX12::CommandEncoder* dxCommandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);
		UINT AccelerationStructureIndex = 0;

		while (AccelerationStructureIndex < AccelerationStructureCount) {
			const BaseBufferView& AccelerationStructureView = accelerationStructureViews[AccelerationStructureIndex];
			const BaseBufferView& ScratchView = scratchViews[AccelerationStructureIndex];

			ref<DX12::AccelerationStructure> accelerationStructure = ref_cast<DX12::AccelerationStructure>(accelerationStructures[AccelerationStructureIndex]);
			accelerationStructure->BuildDesc.DestAccelerationStructureData = pointer_cast<const DX12::Buffer*>(AccelerationStructureView.getBuffer())->resource->GetGPUVirtualAddress() + AccelerationStructureView.getOffset();
			accelerationStructure->BuildDesc.ScratchAccelerationStructureData = pointer_cast<const DX12::Buffer*>(ScratchView.getBuffer())->resource->GetGPUVirtualAddress() + ScratchView.getOffset();

			dxCommandEncoder->commandList->BuildRaytracingAccelerationStructure(&accelerationStructure->BuildDesc, 0, nullptr);

			AccelerationStructureIndex += 1;
		}

		dxCommandEncoder->commandCount += 1;
	}

	void traceRays(Trident::CommandEncoder* commandEncoder, const DeviceAddressRegion& generationShaderBindingTable, const DeviceAddressRegion& missShaderBindingTable, const DeviceAddressRegion& hitShaderBindingTable, const DeviceAddressRegion& callableShaderBindingTable, Extent3D extent) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ENGINE_ERROR_UNIMPLEMENTED();
	}
}