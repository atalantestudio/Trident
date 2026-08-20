// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderVisibility.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::PipelineLayout::PipelineLayout(const Trident::Device* device) :
		Trident::PipelineLayout(device)
	{}

	ref<Trident::PipelineLayout> DX12::createPipelineLayout(const Trident::Device* device, const PipelineLayoutDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		ref<DX12::PipelineLayout> pipelineLayout = allocateRef<DX12::PipelineLayout>(device);
		HRESULT result;

		UINT rootParameterCount = 0;
		UINT rootParameterOffset = 0;

		UINT viewDescriptorRangeCount = 0;
		UINT samplerDescriptorRangeCount = 0;
		UINT descriptorRangeOffset = 0;
		UINT samplerDescriptorRangeOffset = 0;

		UINT constantBufferViewShaderRegister = 0;
		UINT shaderResourceViewShaderRegister = 0;
		UINT unorderedAccessViewShaderRegister = 0;
		UINT samplerShaderRegister = 0;

		ShaderStage samplerStageFlags{};

		for (const ref<DescriptorSetLayout>& descriptorSetLayout : descriptor.descriptorSetLayouts) {
			if (descriptorSetLayout->bindings.count() == 0) {
				continue;
			}

			for (const DescriptorSetLayoutBinding& binding : descriptorSetLayout->bindings) {
				if (binding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER) {
					samplerDescriptorRangeCount += 1;
				}

				viewDescriptorRangeCount += 1;
			}

			rootParameterCount += 1;
		}

		const bool samplerDescriptorTable = samplerDescriptorRangeCount > 0;

		if (samplerDescriptorTable) {
			pipelineLayout->samplerDescriptorTableRootParameterIndex = rootParameterCount;
			samplerDescriptorRangeOffset = viewDescriptorRangeCount;
			rootParameterCount += 1;
		}

		pipelineLayout->samplerOffsetsPerDescriptorSet = sequence<UINT>(descriptor.descriptorSetLayouts.count());

		sequence<D3D12_ROOT_PARAMETER1> rootParameters(rootParameterCount);
		sequence<D3D12_DESCRIPTOR_RANGE1> descriptorRanges(viewDescriptorRangeCount + samplerDescriptorRangeCount);

		UINT samplerOffset = 0;
		UINT descriptorSetLayoutIndex = 0;

		while (descriptorSetLayoutIndex < descriptor.descriptorSetLayouts.count()) {
			const ref<DescriptorSetLayout> descriptorSetLayout = descriptor.descriptorSetLayouts[descriptorSetLayoutIndex];

			if (descriptorSetLayout->bindings.count() == 0) {
				descriptorSetLayoutIndex += 1;

				continue;
			}

			pipelineLayout->samplerOffsetsPerDescriptorSet[descriptorSetLayoutIndex] = samplerOffset;

			const UINT descriptorRangeStart = descriptorRangeOffset;

			ShaderStage viewStageFlags{};

			for (const DescriptorSetLayoutBinding& binding : descriptorSetLayout->bindings) {
				switch (binding.descriptorType) {
					case DescriptorType::COMBINED_IMAGE_SAMPLER: {
						D3D12_DESCRIPTOR_RANGE1& imageDescriptorRange = descriptorRanges[descriptorRangeOffset];
						imageDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
						imageDescriptorRange.NumDescriptors = binding.descriptorCount;
						imageDescriptorRange.BaseShaderRegister = shaderResourceViewShaderRegister;
						imageDescriptorRange.RegisterSpace = 0;
						imageDescriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						imageDescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						shaderResourceViewShaderRegister += binding.descriptorCount;

						D3D12_DESCRIPTOR_RANGE1& samplerDescriptorRange = descriptorRanges[samplerDescriptorRangeOffset];
						samplerDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
						samplerDescriptorRange.NumDescriptors = binding.descriptorCount;
						samplerDescriptorRange.BaseShaderRegister = samplerShaderRegister;
						samplerDescriptorRange.RegisterSpace = 0;
						samplerDescriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						samplerDescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						samplerOffset += 1;
						samplerDescriptorRangeOffset += 1;
						samplerShaderRegister += binding.descriptorCount;
						samplerStageFlags |= binding.stageFlags;

						break;
					}
					case DescriptorType::READ_ONLY_STORAGE_IMAGE: {
						D3D12_DESCRIPTOR_RANGE1& descriptorRange = descriptorRanges[descriptorRangeOffset];
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
						descriptorRange.NumDescriptors = binding.descriptorCount;
						descriptorRange.BaseShaderRegister = shaderResourceViewShaderRegister;
						descriptorRange.RegisterSpace = 0;
						descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						shaderResourceViewShaderRegister += binding.descriptorCount;

						break;
					}
					case DescriptorType::READ_WRITE_STORAGE_IMAGE: {
						D3D12_DESCRIPTOR_RANGE1& descriptorRange = descriptorRanges[descriptorRangeOffset];
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
						descriptorRange.NumDescriptors = binding.descriptorCount;
						descriptorRange.BaseShaderRegister = unorderedAccessViewShaderRegister;
						descriptorRange.RegisterSpace = 0;
						descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						unorderedAccessViewShaderRegister += binding.descriptorCount;

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

						constantBufferViewShaderRegister += binding.descriptorCount;

						break;
					}
					case DescriptorType::READ_ONLY_STORAGE_BUFFER: {
						D3D12_DESCRIPTOR_RANGE1& descriptorRange = descriptorRanges[descriptorRangeOffset];
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
						descriptorRange.NumDescriptors = binding.descriptorCount;
						descriptorRange.BaseShaderRegister = shaderResourceViewShaderRegister;
						descriptorRange.RegisterSpace = 0;
						descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						shaderResourceViewShaderRegister += binding.descriptorCount;

						break;
					}
					case DescriptorType::READ_WRITE_STORAGE_BUFFER: {
						D3D12_DESCRIPTOR_RANGE1& descriptorRange = descriptorRanges[descriptorRangeOffset];
						descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
						descriptorRange.NumDescriptors = binding.descriptorCount;
						descriptorRange.BaseShaderRegister = unorderedAccessViewShaderRegister;

						if (binding.stageFlags & ShaderStage::FRAGMENT) {
							// TODO(graphics/dx12/root-signature/fragment-uav): Handle multiple output registers
							ENGINE_WARNING("UAV registers live in the same name space as outputs, so they must be bound to at least 'u1'. The BaseShaderRegister has been incremented.");

							descriptorRange.BaseShaderRegister += 1;
						}

						descriptorRange.RegisterSpace = 0;
						descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

						unorderedAccessViewShaderRegister += binding.descriptorCount;

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

						shaderResourceViewShaderRegister += binding.descriptorCount;

						break;
					}
					default: {
						ADAPTER_ASSERT(false, "Unsupported descriptor type.");

						break;
					}
				}

				viewStageFlags |= binding.stageFlags;

				descriptorRangeOffset += 1;
			}

			D3D12_ROOT_PARAMETER1& rootParameter = rootParameters[rootParameterOffset];
			rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameter.DescriptorTable.NumDescriptorRanges = descriptorRangeOffset - descriptorRangeStart;
			rootParameter.DescriptorTable.pDescriptorRanges = &descriptorRanges[descriptorRangeStart];
			rootParameter.ShaderVisibility = getShaderVisibility(viewStageFlags);

			rootParameterOffset += 1;
			descriptorSetLayoutIndex += 1;
		}

		if (samplerDescriptorTable) {
			D3D12_ROOT_PARAMETER1& samplerRootParameter = rootParameters[pipelineLayout->samplerDescriptorTableRootParameterIndex];
			samplerRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			samplerRootParameter.DescriptorTable.NumDescriptorRanges = samplerDescriptorRangeCount;
			samplerRootParameter.DescriptorTable.pDescriptorRanges = &descriptorRanges[viewDescriptorRangeCount];
			samplerRootParameter.ShaderVisibility = getShaderVisibility(samplerStageFlags);
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDescriptor{};
		versionedRootSignatureDescriptor.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		versionedRootSignatureDescriptor.Desc_1_1.NumParameters = rootParameterCount;
		versionedRootSignatureDescriptor.Desc_1_1.pParameters = &rootParameters[0];
		versionedRootSignatureDescriptor.Desc_1_1.NumStaticSamplers = 0;
		versionedRootSignatureDescriptor.Desc_1_1.pStaticSamplers = nullptr;
		versionedRootSignatureDescriptor.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ID3DBlob* blob;

		#if defined(BUILD_MODE_DEBUG)
			ID3DBlob* errorBlob;
		#endif

		result = D3D12SerializeVersionedRootSignature(
			&versionedRootSignatureDescriptor,
			&blob,
			#if defined(BUILD_MODE_DEBUG)
				&errorBlob
			#else
				nullptr
			#endif
		);

		if (FAILED(result)) {
			#if defined(BUILD_MODE_DEBUG)
				if (errorBlob) {
					const view<char8> error(pointer_cast<const char8*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());

					ADAPTER_ASSERT(false, "Failed to serialize root signature: []", error);

					errorBlob->Release();
				}
			#else
				ADAPTER_ASSERT(false, "Failed to serialize root signature.");
			#endif
		}

		result = _device->device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&pipelineLayout->rootSignature));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create root signature.");

		#if defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				const LPWSTR name = ConvertUTF8ToUTF16(&descriptor.name[0]);

				result = pipelineLayout->rootSignature->SetName(name);

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set root signature name.");

				free(name);
			}
		#endif

		blob->Release();

		return pipelineLayout;
	}

	void DX12::destroyPipelineLayout(const Trident::Device* device, const Trident::PipelineLayout* pipelineLayout) {
		const DX12::PipelineLayout* _pipelineLayout = static_cast<const DX12::PipelineLayout*>(pipelineLayout);

		_pipelineLayout->rootSignature->Release();
	}
}