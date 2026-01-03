// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::PipelineLayout::PipelineLayout(const PipelineLayoutDescriptor& descriptor, const DX12::Device& device) :
		ProjectA::PipelineLayout(descriptor)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;

		// TODO: Add support for root signature parameters.
		ADAPTER_ASSERT(descriptor.descriptorSetLayouts.count() == 0, "Descriptor set layouts are not currently supported.");

		D3D12_ROOT_SIGNATURE_DESC rootSignatureDescriptor{};
		rootSignatureDescriptor.NumParameters = 0;
		rootSignatureDescriptor.pParameters = nullptr;
		rootSignatureDescriptor.NumStaticSamplers = 0;
		rootSignatureDescriptor.pStaticSamplers = nullptr;
		rootSignatureDescriptor.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> blob;

		result = D3D12SerializeRootSignature(&rootSignatureDescriptor, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to serialize root signature.");

		result = device.device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create root signature.");
	}

	DX12::PipelineLayout::~PipelineLayout() {
		ENGINE_ERROR_UNIMPLEMENTED();
	}
}