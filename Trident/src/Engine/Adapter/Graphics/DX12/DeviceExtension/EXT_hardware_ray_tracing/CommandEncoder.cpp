// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/RayTracingPipeline.hpp"

namespace Trident::DX12 {
	void bindRayTracingPipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::RayTracingPipeline> pipeline) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const ref<DX12::RayTracingPipeline> dxPipeline = ref_cast<DX12::RayTracingPipeline>(pipeline);

		DX12::CommandEncoder* dxCommandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		dxCommandEncoder->commandList->SetPipelineState1(dxPipeline->stateObject);
		dxCommandEncoder->commandList->SetComputeRootSignature(dxPipeline->rootSignature);
		dxCommandEncoder->computePipelineBound = true;
		dxCommandEncoder->commandCount += 1;
	}

	void buildAccelerationStructures(Trident::CommandEncoder* commandEncoder, view<ref<Trident::AccelerationStructure>> accelerationStructures, view<BaseBufferView> scratchViews) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Device* dxDevice = static_cast<const DX12::Device*>(commandEncoder->device);
		const UINT AccelerationStructureCount = static_cast<UINT>(accelerationStructures.count());

		DX12::CommandEncoder* dxCommandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);
		UINT AccelerationStructureIndex = 0;

		while (AccelerationStructureIndex < AccelerationStructureCount) {
			const BaseBufferView& ScratchView = scratchViews[AccelerationStructureIndex];

			ref<DX12::AccelerationStructure> accelerationStructure = ref_cast<DX12::AccelerationStructure>(accelerationStructures[AccelerationStructureIndex]);
			accelerationStructure->BuildDesc.ScratchAccelerationStructureData = pointer_cast<const DX12::Buffer*>(ScratchView.getBuffer())->resource->GetGPUVirtualAddress() + ScratchView.getOffset();

			dxCommandEncoder->commandList->BuildRaytracingAccelerationStructure(&accelerationStructure->BuildDesc, 0, nullptr);

			AccelerationStructureIndex += 1;
		}

		dxCommandEncoder->commandCount += 1;
	}

	void traceRays(Trident::CommandEncoder* commandEncoder, const DeviceAddressRegion& generationShaderBindingTable, const DeviceAddressRegion& missShaderBindingTable, const DeviceAddressRegion& hitShaderBindingTable, const DeviceAddressRegion& callableShaderBindingTable, Extent3D extent) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* dxCommandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		D3D12_DISPATCH_RAYS_DESC Desc{};
		Desc.RayGenerationShaderRecord.StartAddress = generationShaderBindingTable.deviceAddress;
		Desc.RayGenerationShaderRecord.SizeInBytes = generationShaderBindingTable.size;

		// TODO(graphics/dx12/ext_hardware_ray_tracing/trace-rays): Use the stride of the ray generation SBT.

		Desc.MissShaderTable.StartAddress = missShaderBindingTable.deviceAddress;
		Desc.MissShaderTable.SizeInBytes = missShaderBindingTable.size;
		Desc.MissShaderTable.StrideInBytes = missShaderBindingTable.stride;
		Desc.HitGroupTable.StartAddress = hitShaderBindingTable.deviceAddress;
		Desc.HitGroupTable.SizeInBytes = hitShaderBindingTable.size;
		Desc.HitGroupTable.StrideInBytes = hitShaderBindingTable.stride;
		Desc.CallableShaderTable.StartAddress = callableShaderBindingTable.deviceAddress;
		Desc.CallableShaderTable.SizeInBytes = callableShaderBindingTable.size;
		Desc.CallableShaderTable.StrideInBytes = callableShaderBindingTable.stride;
		Desc.Width = extent.width;
		Desc.Height = extent.height;
		Desc.Depth = extent.depth;

		dxCommandEncoder->commandList->DispatchRays(&Desc);
		dxCommandEncoder->commandCount += 1;
	}
}