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
		UINT rootParameterOffset = 0;
		UINT samplerRootParameterIndex = 0;

		UINT viewDescriptorRangeCount = 0;
		UINT samplerDescriptorRangeCount = 0;
		UINT descriptorRangeOffset = 0;
		UINT samplerDescriptorRangeOffset = 0;

		UINT constantBufferViewShaderRegister = 0;
		UINT shaderResourceViewShaderRegister = 0;
		UINT samplerShaderRegister = 0;

		ShaderStage samplerShaderStages{};

		for (const ref<DescriptorSetLayout> descriptorSetLayout : descriptor.descriptorSetLayouts) {
			for (const DescriptorSetLayoutBinding& binding : descriptorSetLayout->bindings) {
				if (binding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER) {
					samplerDescriptorRangeCount += 1;
				}

				viewDescriptorRangeCount += 1;
			}

			rootParameterCount += 1;
		}

		if (samplerDescriptorRangeCount > 0) {
			samplerRootParameterIndex = rootParameterCount;
			samplerDescriptorRangeOffset = viewDescriptorRangeCount;
			rootParameterCount += 1;
		}

		sequence<D3D12_ROOT_PARAMETER1> rootParameters(rootParameterCount);
		sequence<D3D12_DESCRIPTOR_RANGE1> descriptorRanges(viewDescriptorRangeCount + samplerDescriptorRangeCount);

		for (const ref<DescriptorSetLayout> descriptorSetLayout : descriptor.descriptorSetLayouts) {
			const UINT descriptorRangeStart = descriptorRangeOffset;
			ShaderStage viewShaderStages{};

			for (const DescriptorSetLayoutBinding& binding : descriptorSetLayout->bindings) {
				viewShaderStages |= binding.shaderStages;

				switch (binding.descriptorType) {
					case DescriptorType::COMBINED_IMAGE_SAMPLER: {
						D3D12_DESCRIPTOR_RANGE1& imageDescriptorRange = descriptorRanges[descriptorRangeOffset];
						imageDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
						imageDescriptorRange.NumDescriptors = binding.descriptorCount;
						imageDescriptorRange.BaseShaderRegister = shaderResourceViewShaderRegister;
						imageDescriptorRange.RegisterSpace = 0;
						imageDescriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						imageDescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						shaderResourceViewShaderRegister += 1;

						D3D12_DESCRIPTOR_RANGE1& samplerDescriptorRange = descriptorRanges[samplerDescriptorRangeOffset];
						samplerDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
						samplerDescriptorRange.NumDescriptors = binding.descriptorCount;
						samplerDescriptorRange.BaseShaderRegister = samplerShaderRegister;
						samplerDescriptorRange.RegisterSpace = 0;
						samplerDescriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						samplerDescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						samplerDescriptorRangeOffset += 1;
						samplerShaderRegister += 1;
						samplerShaderStages |= binding.shaderStages;

						break;
					}
					case DescriptorType::STORAGE_IMAGE: {
						D3D12_DESCRIPTOR_RANGE1& descriptorRange = descriptorRanges[descriptorRangeOffset];
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
						descriptorRange.NumDescriptors = binding.descriptorCount;
						descriptorRange.BaseShaderRegister = shaderResourceViewShaderRegister;
						descriptorRange.RegisterSpace = 0;
						descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						shaderResourceViewShaderRegister += 1;

						break;
					}
					case DescriptorType::UNIFORM_BUFFER: {
						D3D12_DESCRIPTOR_RANGE1& descriptorRange = descriptorRanges[descriptorRangeOffset];
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
						descriptorRange.NumDescriptors = binding.descriptorCount;
						descriptorRange.BaseShaderRegister = constantBufferViewShaderRegister;
						descriptorRange.RegisterSpace = 0;
						descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						constantBufferViewShaderRegister += 1;

						break;
					}
					case DescriptorType::STORAGE_BUFFER: {
						D3D12_DESCRIPTOR_RANGE1& descriptorRange = descriptorRanges[descriptorRangeOffset];
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
						descriptorRange.NumDescriptors = binding.descriptorCount;
						descriptorRange.BaseShaderRegister = shaderResourceViewShaderRegister;
						descriptorRange.RegisterSpace = 0;
						descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						shaderResourceViewShaderRegister += 1;

						break;
					}
					case DescriptorType::INPUT_ATTACHMENT: {
						D3D12_DESCRIPTOR_RANGE1& descriptorRange = descriptorRanges[descriptorRangeOffset];
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
						descriptorRange.NumDescriptors = binding.descriptorCount;
						descriptorRange.BaseShaderRegister = shaderResourceViewShaderRegister;
						descriptorRange.RegisterSpace = 0;
						descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						shaderResourceViewShaderRegister += 1;

						break;
					}
					default: {
						ADAPTER_ASSERT(false, "Unsupported descriptor type.");

						break;
					}
				}

				descriptorRangeOffset += 1;
			}

			D3D12_ROOT_PARAMETER1& rootParameter = rootParameters[rootParameterOffset];
			rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameter.DescriptorTable.NumDescriptorRanges = descriptorRangeOffset - descriptorRangeStart;
			rootParameter.DescriptorTable.pDescriptorRanges = &descriptorRanges[descriptorRangeStart];
			rootParameter.ShaderVisibility = getShaderVisibility(viewShaderStages);

			rootParameterOffset += 1;
		}

		if (samplerDescriptorRangeCount > 0) {
			D3D12_ROOT_PARAMETER1& samplerRootParameter = rootParameters[samplerRootParameterIndex];
			samplerRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			samplerRootParameter.DescriptorTable.NumDescriptorRanges = samplerDescriptorRangeCount;
			samplerRootParameter.DescriptorTable.pDescriptorRanges = &descriptorRanges[viewDescriptorRangeCount];
			samplerRootParameter.ShaderVisibility = getShaderVisibility(samplerShaderStages);
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDescriptor{};
		versionedRootSignatureDescriptor.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		versionedRootSignatureDescriptor.Desc_1_1.NumParameters = rootParameterCount;
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