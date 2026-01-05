// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/BlendFactor.hpp"
#include "Engine/Adapter/Graphics/DX12/CullMode.hpp"
#include "Engine/Adapter/Graphics/DX12/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/PrimitiveTopology.hpp"
#include "Engine/Adapter/Graphics/DX12/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/DX12/VertexInputAttributeType.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::RenderPipeline::RenderPipeline(const RenderPipelineDescriptor& descriptor, const DX12::Device& device) :
		ProjectA::RenderPipeline(descriptor)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ADAPTER_ASSERT(descriptor.colorBlendState.attachments.size() <= 8 && descriptor.renderPass->getAttachmentCount() <= 8, "Too many color blend attachments.");

		HRESULT result;

		const ref<DX12::PipelineLayout> pipelineLayout = std::static_pointer_cast<DX12::PipelineLayout>(descriptor.pipelineLayout);
		const ref<DX12::ShaderModule> vertexShaderModule = std::static_pointer_cast<DX12::ShaderModule>(descriptor.vertexShaderModule);
		const ref<DX12::ShaderModule> fragmentShaderModule = std::static_pointer_cast<DX12::ShaderModule>(descriptor.fragmentShaderModule);
		const ref<RenderPass>& renderPass = descriptor.renderPass;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDescriptor{};
		graphicsPipelineStateDescriptor.pRootSignature = pipelineLayout->rootSignature.Get();
		graphicsPipelineStateDescriptor.VS.pShaderBytecode = vertexShaderModule->blob->GetBufferPointer();
		graphicsPipelineStateDescriptor.VS.BytecodeLength = vertexShaderModule->blob->GetBufferSize();
		graphicsPipelineStateDescriptor.PS.pShaderBytecode = fragmentShaderModule->blob->GetBufferPointer();
		graphicsPipelineStateDescriptor.PS.BytecodeLength = fragmentShaderModule->blob->GetBufferSize();
		graphicsPipelineStateDescriptor.DS.pShaderBytecode = nullptr;
		graphicsPipelineStateDescriptor.DS.BytecodeLength = 0;
		graphicsPipelineStateDescriptor.HS.pShaderBytecode = nullptr;
		graphicsPipelineStateDescriptor.HS.BytecodeLength = 0;
		graphicsPipelineStateDescriptor.GS.pShaderBytecode = nullptr;
		graphicsPipelineStateDescriptor.GS.BytecodeLength = 0;

		// TODO: Set correct stream output.
		graphicsPipelineStateDescriptor.StreamOutput.pSODeclaration = nullptr;
		graphicsPipelineStateDescriptor.StreamOutput.NumEntries = 0;
		graphicsPipelineStateDescriptor.StreamOutput.pBufferStrides = nullptr;
		graphicsPipelineStateDescriptor.StreamOutput.NumStrides = 0;
		graphicsPipelineStateDescriptor.StreamOutput.RasterizedStream = 0;

		graphicsPipelineStateDescriptor.BlendState.AlphaToCoverageEnable = FALSE;
		graphicsPipelineStateDescriptor.BlendState.IndependentBlendEnable = FALSE;

		uint32 renderTargetBlendDescriptorIndex = 0;

		while (renderTargetBlendDescriptorIndex < descriptor.colorBlendState.attachments.size()) {
			const ColorBlendAttachment& colorBlendAttachment = descriptor.colorBlendState.attachments[renderTargetBlendDescriptorIndex];

			D3D12_RENDER_TARGET_BLEND_DESC& renderTargetBlendDescriptor = graphicsPipelineStateDescriptor.BlendState.RenderTarget[renderTargetBlendDescriptorIndex];
			renderTargetBlendDescriptor.BlendEnable = colorBlendAttachment.blend;
			renderTargetBlendDescriptor.LogicOpEnable = FALSE;
			renderTargetBlendDescriptor.SrcBlend = getBlendFactor(colorBlendAttachment.srcColorBlendFactor);
			renderTargetBlendDescriptor.DestBlend = getBlendFactor(colorBlendAttachment.dstColorBlendFactor);
			renderTargetBlendDescriptor.BlendOp = D3D12_BLEND_OP_ADD;
			renderTargetBlendDescriptor.SrcBlendAlpha = D3D12_BLEND_ONE;
			renderTargetBlendDescriptor.DestBlendAlpha = D3D12_BLEND_ZERO;
			renderTargetBlendDescriptor.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			renderTargetBlendDescriptor.LogicOp = D3D12_LOGIC_OP_NOOP;
			renderTargetBlendDescriptor.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			renderTargetBlendDescriptorIndex += 1;
		}

		graphicsPipelineStateDescriptor.SampleMask = max<UINT>();

		graphicsPipelineStateDescriptor.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		graphicsPipelineStateDescriptor.RasterizerState.CullMode = getCullMode(descriptor.cullMode);
		graphicsPipelineStateDescriptor.RasterizerState.FrontCounterClockwise = descriptor.frontFace == FrontFace::COUNTER_CLOCKWISE;
		graphicsPipelineStateDescriptor.RasterizerState.DepthBias = 0;
		graphicsPipelineStateDescriptor.RasterizerState.DepthBiasClamp = 0.0f;
		graphicsPipelineStateDescriptor.RasterizerState.SlopeScaledDepthBias = 0.0f;
		graphicsPipelineStateDescriptor.RasterizerState.DepthClipEnable = FALSE;

		// TODO: Set correct multisample state (depends on the render pass).
		graphicsPipelineStateDescriptor.RasterizerState.MultisampleEnable = FALSE;

		graphicsPipelineStateDescriptor.RasterizerState.AntialiasedLineEnable = FALSE;
		graphicsPipelineStateDescriptor.RasterizerState.ForcedSampleCount = 0;
		graphicsPipelineStateDescriptor.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		graphicsPipelineStateDescriptor.DepthStencilState.DepthEnable = descriptor.depthStencilState.depthTest;
		graphicsPipelineStateDescriptor.DepthStencilState.DepthWriteMask = static_cast<D3D12_DEPTH_WRITE_MASK>(descriptor.depthStencilState.depthWrite);
		graphicsPipelineStateDescriptor.DepthStencilState.DepthFunc = static_cast<D3D12_COMPARISON_FUNC>(static_cast<uint8>(descriptor.depthStencilState.compareOperation) + 1);
		graphicsPipelineStateDescriptor.DepthStencilState.StencilEnable = FALSE;
		graphicsPipelineStateDescriptor.DepthStencilState.StencilReadMask = 0;
		graphicsPipelineStateDescriptor.DepthStencilState.StencilWriteMask = 0;
		graphicsPipelineStateDescriptor.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		graphicsPipelineStateDescriptor.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		graphicsPipelineStateDescriptor.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		graphicsPipelineStateDescriptor.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		graphicsPipelineStateDescriptor.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		graphicsPipelineStateDescriptor.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		graphicsPipelineStateDescriptor.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		graphicsPipelineStateDescriptor.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		uint32 inputElementDescriptorIndex = 0;
		uint32 vertexInputBindingIndex = 0;
		uint32 vertexInputAttributeIndex = 0;

		while (vertexInputBindingIndex < descriptor.vertexInputBindings.count()) {
			const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[vertexInputBindingIndex];

			inputElementDescriptorIndex += static_cast<uint32>(vertexInputBinding.getAttributes().size());

			vertexInputBindingIndex += 1;
		}

		sequence<D3D12_INPUT_ELEMENT_DESC> inputElementDescriptors(inputElementDescriptorIndex);

		inputElementDescriptorIndex = 0;
		vertexInputBindingIndex = 0;

		while (vertexInputBindingIndex < descriptor.vertexInputBindings.count()) {
			const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[vertexInputBindingIndex];
			const std::vector<VertexInputAttribute>& vertexInputAttributes = vertexInputBinding.getAttributes();

			vertexInputAttributeIndex = 0;

			while (vertexInputAttributeIndex < vertexInputAttributes.size()) {
				const VertexInputAttribute& vertexInputAttribute = vertexInputAttributes[vertexInputAttributeIndex];

				D3D12_INPUT_ELEMENT_DESC& inputElementDescriptor = inputElementDescriptors[inputElementDescriptorIndex];

				// TODO (debug-only)
				switch (vertexInputAttributeIndex) {
					case 0: {
						inputElementDescriptor.SemanticName = "POSITION";

						break;
					}
					case 1: {
						inputElementDescriptor.SemanticName = "COLOR";

						break;
					}
					default: {
						inputElementDescriptor.SemanticName = "";

						break;
					}
				}

				inputElementDescriptor.SemanticIndex = 0;

				inputElementDescriptor.Format = getVertexInputAttributeType(vertexInputAttribute.type);
				inputElementDescriptor.InputSlot = vertexInputBindingIndex;
				inputElementDescriptor.AlignedByteOffset = vertexInputAttribute.offset;
				inputElementDescriptor.InputSlotClass = static_cast<D3D12_INPUT_CLASSIFICATION>(vertexInputBinding.getInputRate());
				inputElementDescriptor.InstanceDataStepRate = 0;

				inputElementDescriptorIndex += 1;
				vertexInputAttributeIndex += 1;
			}

			vertexInputBindingIndex += 1;
		}

		graphicsPipelineStateDescriptor.InputLayout.pInputElementDescs = &inputElementDescriptors[0];
		graphicsPipelineStateDescriptor.InputLayout.NumElements = static_cast<UINT>(inputElementDescriptors.count());

		graphicsPipelineStateDescriptor.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		graphicsPipelineStateDescriptor.PrimitiveTopologyType = getPrimitiveTopologyType(descriptor.primitiveTopology);

		graphicsPipelineStateDescriptor.NumRenderTargets = renderPass->getAttachmentCount();

		uint32 attachmentIndex = 0;

		while (attachmentIndex < renderPass->getAttachmentCount()) {
			const Attachment& attachment = renderPass->getAttachment(attachmentIndex);

			graphicsPipelineStateDescriptor.RTVFormats[attachmentIndex] = getFormat(attachment.format);

			attachmentIndex += 1;
		}

		graphicsPipelineStateDescriptor.DSVFormat = DXGI_FORMAT_UNKNOWN;

		// TODO: Set correct sample count (depends on the render pass).
		graphicsPipelineStateDescriptor.SampleDesc.Count = 1;

		graphicsPipelineStateDescriptor.SampleDesc.Quality = 0;
		graphicsPipelineStateDescriptor.NodeMask = 0;
		graphicsPipelineStateDescriptor.CachedPSO.pCachedBlob = nullptr;
		graphicsPipelineStateDescriptor.CachedPSO.CachedBlobSizeInBytes = 0;
		graphicsPipelineStateDescriptor.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		result = device.device->CreateGraphicsPipelineState(&graphicsPipelineStateDescriptor, IID_PPV_ARGS(&pipelineState));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create graphics pipeline state.");
	}
}