// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/Descriptor/ComputePipelineDescriptor.hpp"

namespace Trident {
	DX12::ComputePipeline::ComputePipeline(const Trident::Device* device) :
		Trident::ComputePipeline(device)
	{}

	ref<Trident::ComputePipeline> DX12::createComputePipeline(const Trident::Device* device, const ComputePipelineDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		const ref<DX12::ShaderModule> computeShaderModule = std::static_pointer_cast<DX12::ShaderModule>(descriptor.computeShaderModule);
		HRESULT result;

		ref<DX12::ComputePipeline> computePipeline = createRef<DX12::ComputePipeline>(device);
		computePipeline->pipelineLayout = std::static_pointer_cast<DX12::PipelineLayout>(descriptor.pipelineLayout);

		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDescriptor{};
		computePipelineStateDescriptor.pRootSignature = computePipeline->pipelineLayout->rootSignature;
		computePipelineStateDescriptor.CS.pShaderBytecode = computeShaderModule->blob->GetBufferPointer();
		computePipelineStateDescriptor.CS.BytecodeLength = computeShaderModule->blob->GetBufferSize();
		computePipelineStateDescriptor.NodeMask = 0;
		computePipelineStateDescriptor.CachedPSO.pCachedBlob = nullptr;
		computePipelineStateDescriptor.CachedPSO.CachedBlobSizeInBytes = 0;
		computePipelineStateDescriptor.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		result = _device->device->CreateComputePipelineState(&computePipelineStateDescriptor, IID_PPV_ARGS(&computePipeline->pipelineState));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create compute pipeline state.");

		#if defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				const LPWSTR name = ConvertUTF8ToUTF16(&descriptor.name[0]);

				result = computePipeline->pipelineState->SetName(name);

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set pipeline state name.");

				free(name);
			}
		#endif

		return computePipeline;
	}

	void DX12::destroyComputePipeline(const Trident::Device* device, const Trident::ComputePipeline* computePipeline) {
		const DX12::ComputePipeline* _computePipeline = static_cast<const DX12::ComputePipeline*>(computePipeline);

		_computePipeline->pipelineState->Release();
	}
}