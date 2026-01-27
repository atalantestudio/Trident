// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::ComputePipeline::ComputePipeline(const ComputePipelineDescriptor& descriptor, const DX12::Device& device) :
		Trident::ComputePipeline(descriptor),
		pipelineLayout(std::static_pointer_cast<DX12::PipelineLayout>(descriptor.pipelineLayout))
	{
		const ref<DX12::ShaderModule> computeShaderModule = std::static_pointer_cast<DX12::ShaderModule>(descriptor.computeShaderModule);

		HRESULT result;

		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDescriptor{};
		computePipelineStateDescriptor.pRootSignature = pipelineLayout->rootSignature.Get();
		computePipelineStateDescriptor.CS.pShaderBytecode = computeShaderModule->blob->GetBufferPointer();
		computePipelineStateDescriptor.CS.BytecodeLength = computeShaderModule->blob->GetBufferSize();
		computePipelineStateDescriptor.NodeMask = 0;
		computePipelineStateDescriptor.CachedPSO.pCachedBlob = nullptr;
		computePipelineStateDescriptor.CachedPSO.CachedBlobSizeInBytes = 0;
		computePipelineStateDescriptor.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		result = device.device->CreateComputePipelineState(&computePipelineStateDescriptor, IID_PPV_ARGS(&pipelineState));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create compute pipeline state.");
	}
}