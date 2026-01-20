// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/DescriptorRangeType.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderVisibility.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::PipelineLayout::PipelineLayout(const PipelineLayoutDescriptor& descriptor, const DX12::Device& device) :
		ProjectA::PipelineLayout(descriptor)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;
		UINT rootParameterCount = 0;
		UINT descriptorRangeCount = 0;

		for (const ref<DescriptorSetLayout> descriptorSetLayout : descriptor.descriptorSetLayouts) {
			for (const DescriptorSetLayoutBinding& binding : descriptorSetLayout->bindings) {
				if (binding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER) {
					// Samplers and shader resource views cannot be mixed within a descriptor table.
					rootParameterCount += 1;
					descriptorRangeCount += 1;
				}

				rootParameterCount += 1;
				descriptorRangeCount += 1;
			}
		}

		sequence<D3D12_ROOT_PARAMETER1> rootParameters(rootParameterCount);
		sequence<D3D12_DESCRIPTOR_RANGE1> descriptorRanges(descriptorRangeCount);

		UINT rootParameterIndex = 0;
		UINT descriptorRangeIndex = 0;
		UINT bindingIndex = 0;

		for (const ref<DescriptorSetLayout> descriptorSetLayout : descriptor.descriptorSetLayouts) {
			bindingIndex = 0;

			while (bindingIndex < descriptorSetLayout->bindings.count()) {
				const DescriptorSetLayoutBinding& binding = descriptorSetLayout->bindings[bindingIndex];
				const D3D12_SHADER_VISIBILITY shaderVisibility = getShaderVisibility(binding.shaderStages);

				switch (binding.descriptorType) {
					case DescriptorType::COMBINED_IMAGE_SAMPLER: {
						D3D12_DESCRIPTOR_RANGE1& imageDescriptorRange = descriptorRanges[descriptorRangeIndex];
						imageDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
						imageDescriptorRange.NumDescriptors = binding.descriptorCount;
						imageDescriptorRange.BaseShaderRegister = bindingIndex;
						imageDescriptorRange.RegisterSpace = 0;
						imageDescriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						imageDescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						D3D12_ROOT_PARAMETER1& imageRootParameter = rootParameters[rootParameterIndex];
						imageRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
						imageRootParameter.DescriptorTable.NumDescriptorRanges = 1;
						imageRootParameter.DescriptorTable.pDescriptorRanges = &imageDescriptorRange;
						imageRootParameter.ShaderVisibility = shaderVisibility;

						rootParameterIndex += 1;
						descriptorRangeIndex += 1;

						D3D12_DESCRIPTOR_RANGE1& samplerDescriptorRange = descriptorRanges[descriptorRangeIndex];
						samplerDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
						samplerDescriptorRange.NumDescriptors = binding.descriptorCount;
						samplerDescriptorRange.BaseShaderRegister = bindingIndex;
						samplerDescriptorRange.RegisterSpace = 0;
						samplerDescriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						samplerDescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						D3D12_ROOT_PARAMETER1& samplerRootParameter = rootParameters[rootParameterIndex];
						samplerRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
						samplerRootParameter.DescriptorTable.NumDescriptorRanges = 1;
						samplerRootParameter.DescriptorTable.pDescriptorRanges = &samplerDescriptorRange;
						samplerRootParameter.ShaderVisibility = shaderVisibility;

						break;
					}
					case DescriptorType::STORAGE_IMAGE: {
						D3D12_DESCRIPTOR_RANGE1& imageDescriptorRange = descriptorRanges[descriptorRangeIndex];
						imageDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
						imageDescriptorRange.NumDescriptors = binding.descriptorCount;
						imageDescriptorRange.BaseShaderRegister = bindingIndex;
						imageDescriptorRange.RegisterSpace = 0;
						imageDescriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						imageDescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						D3D12_ROOT_PARAMETER1& imageRootParameter = rootParameters[rootParameterIndex];
						imageRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
						imageRootParameter.DescriptorTable.NumDescriptorRanges = 1;
						imageRootParameter.DescriptorTable.pDescriptorRanges = &imageDescriptorRange;
						imageRootParameter.ShaderVisibility = shaderVisibility;

						break;
					}
					default: {
						ADAPTER_ASSERT(false, "Unsupported descriptor type.");

						break;
					}
				}

				rootParameterIndex += 1;
				descriptorRangeIndex += 1;
				bindingIndex += 1;
			}
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDescriptor{};
		versionedRootSignatureDescriptor.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
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