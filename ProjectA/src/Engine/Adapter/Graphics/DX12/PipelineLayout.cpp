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
		UINT rootParameterCount = 0;
		UINT descriptorRangeCount = 0;
		UINT samplerDescriptorRangeCount = 0;

		while (rootParameterCount < descriptor.descriptorSetLayouts.count()) {
			const ref<DescriptorSetLayout> descriptorSetLayout = descriptor.descriptorSetLayouts[rootParameterCount];

			UINT bindingIndex = 0;

			while (bindingIndex < descriptorSetLayout->bindings.count()) {
				const DescriptorSetLayoutBinding& binding = descriptorSetLayout->bindings[bindingIndex];

				if (binding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER) {
					samplerDescriptorRangeCount += 1;
				}

				descriptorRangeCount += 1;
				bindingIndex += 1;
			}

			rootParameterCount += 1;
		}

		if (samplerDescriptorRangeCount > 0) {
			// Add a descriptor table for samplers.
			rootParameterCount += 1;
		}

		sequence<D3D12_ROOT_PARAMETER1> rootParameters(rootParameterCount);
		sequence<D3D12_DESCRIPTOR_RANGE1> descriptorRanges(descriptorRangeCount);
		sequence<D3D12_DESCRIPTOR_RANGE1> samplerDescriptorRanges(samplerDescriptorRangeCount);

		UINT rootParameterOffset = 0;

		if (samplerDescriptorRangeCount > 0) {
			D3D12_ROOT_PARAMETER1& samplerDescriptorTableRootParameter = rootParameters[0];
			samplerDescriptorTableRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			samplerDescriptorTableRootParameter.DescriptorTable.NumDescriptorRanges = samplerDescriptorRangeCount;
			samplerDescriptorTableRootParameter.DescriptorTable.pDescriptorRanges = &samplerDescriptorRanges[0];

			rootParameterOffset = 1;
		}

		rootParameterCount = 0;
		descriptorRangeCount = 0;
		samplerDescriptorRangeCount = 0;

		// TODO(cbv-descriptor-range)
		#if 0
			descriptorRanges = sequence<D3D12_DESCRIPTOR_RANGE1>(descriptorRanges.count() + 1);

			D3D12_DESCRIPTOR_RANGE1& descriptorRange = descriptorRanges[0];
			descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			descriptorRange.NumDescriptors = 1;
			descriptorRange.BaseShaderRegister = 0;
			descriptorRange.RegisterSpace = 0;
			descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
			descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			rootParameters = sequence<D3D12_ROOT_PARAMETER1>(rootParameters.count() + 1);

			D3D12_ROOT_PARAMETER1& rootParameter = rootParameters[0];
			rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameter.Constants.ShaderRegister = 0;
			rootParameter.Constants.RegisterSpace = 0;
			rootParameter.Constants.Num32BitValues = 1;

			rootParameterCount = 1;
			descriptorRangeCount = 1;
		#endif

		while (rootParameterCount < descriptor.descriptorSetLayouts.count()) {
			const ref<DescriptorSetLayout> descriptorSetLayout = descriptor.descriptorSetLayouts[rootParameterCount];

			UINT descriptorRangeOffset = 0;

			while (descriptorRangeOffset < descriptorSetLayout->bindings.count()) {
				const DescriptorSetLayoutBinding& binding = descriptorSetLayout->bindings[descriptorRangeOffset];

				if (binding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER) {
					D3D12_DESCRIPTOR_RANGE1& shaderResourceViewDescriptorRange = descriptorRanges[descriptorRangeCount + descriptorRangeOffset];
					shaderResourceViewDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
					shaderResourceViewDescriptorRange.NumDescriptors = binding.descriptorCount;
					shaderResourceViewDescriptorRange.BaseShaderRegister = 0;
					shaderResourceViewDescriptorRange.RegisterSpace = 0;
					shaderResourceViewDescriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
					shaderResourceViewDescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

					D3D12_DESCRIPTOR_RANGE1& samplerDescriptorRange = samplerDescriptorRanges[samplerDescriptorRangeCount];
					samplerDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
					samplerDescriptorRange.NumDescriptors = binding.descriptorCount;
					samplerDescriptorRange.BaseShaderRegister = 0;
					samplerDescriptorRange.RegisterSpace = 0;
					samplerDescriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
					samplerDescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

					samplerDescriptorRangeCount += 1;
				}
				else {
					ENGINE_ERROR_UNIMPLEMENTED();
				}

				descriptorRangeCount += 1;
				descriptorRangeOffset += 1;
			}

			D3D12_ROOT_PARAMETER1& rootParameter = rootParameters[rootParameterOffset + rootParameterCount];
			rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameter.DescriptorTable.NumDescriptorRanges = descriptorRangeOffset;
			rootParameter.DescriptorTable.pDescriptorRanges = &descriptorRanges[descriptorRangeCount - descriptorRangeOffset];

			rootParameterCount += 1;
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDescriptor{};
		versionedRootSignatureDescriptor.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		versionedRootSignatureDescriptor.Desc_1_1.NumParameters = static_cast<UINT>(rootParameters.count());
		versionedRootSignatureDescriptor.Desc_1_1.pParameters = &rootParameters[0];
		versionedRootSignatureDescriptor.Desc_1_1.NumStaticSamplers = 0;
		versionedRootSignatureDescriptor.Desc_1_1.pStaticSamplers = nullptr;
		versionedRootSignatureDescriptor.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> blob;

		#ifdef BUILD_MODE_DEBUG
			ComPtr<ID3DBlob> errorBlob;
		#endif

		result = D3D12SerializeVersionedRootSignature(
			&versionedRootSignatureDescriptor,
			&blob,
			#ifdef BUILD_MODE_DEBUG
				&errorBlob
			#else
				nullptr
			#endif
		);

		if (FAILED(result)) {
			#ifdef BUILD_MODE_DEBUG
				if (errorBlob) {
					const view<char8> error(pointer_cast<const char8*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());

					ADAPTER_ASSERT(false, "Failed to serialize root signature: []", error);
				}
			#else
				ADAPTER_ASSERT(false, "Failed to serialize root signature.");
			#endif
		}

		result = device.device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create root signature.");
	}
}