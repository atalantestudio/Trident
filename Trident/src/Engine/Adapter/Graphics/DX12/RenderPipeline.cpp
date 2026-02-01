// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/BlendFactor.hpp"
#include "Engine/Adapter/Graphics/DX12/CullMode.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/PrimitiveTopology.hpp"
#include "Engine/Adapter/Graphics/DX12/PrimitiveTopologyType.hpp"
#include "Engine/Adapter/Graphics/DX12/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/DX12/VertexInputAttributeType.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::RenderPipeline::RenderPipeline(const RenderPipelineDescriptor& descriptor, const DX12::Device& device) :
		Trident::RenderPipeline(descriptor),
		pipelineLayout(std::static_pointer_cast<DX12::PipelineLayout>(descriptor.pipelineLayout)),
		primitiveTopology(getPrimitiveTopology(descriptor.primitiveTopology))
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;

		const ref<Subpass> subpass = descriptor.renderPass->getSubpass(descriptor.subpassIndex);
		const ref<DX12::ShaderModule> vertexShaderModule = std::static_pointer_cast<DX12::ShaderModule>(descriptor.vertexShaderModule);
		const ref<DX12::ShaderModule> fragmentShaderModule = std::static_pointer_cast<DX12::ShaderModule>(descriptor.fragmentShaderModule);

		ADAPTER_ASSERT(subpass->getColorAttachmentReferenceCount() <= 8, "Too many color attachment references.");
		ADAPTER_ASSERT(descriptor.colorBlendState.attachments.count() <= 8, "Too many color blend attachments.");

		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDescriptor{};
		graphicsPipelineStateDescriptor.pRootSignature = pipelineLayout->rootSignature;
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
		graphicsPipelineStateDescriptor.StreamOutput.pSODeclaration = nullptr;
		graphicsPipelineStateDescriptor.StreamOutput.NumEntries = 0;
		graphicsPipelineStateDescriptor.StreamOutput.pBufferStrides = nullptr;
		graphicsPipelineStateDescriptor.StreamOutput.NumStrides = 0;
		graphicsPipelineStateDescriptor.StreamOutput.RasterizedStream = 0;

		graphicsPipelineStateDescriptor.BlendState.AlphaToCoverageEnable = FALSE;
		graphicsPipelineStateDescriptor.BlendState.IndependentBlendEnable = FALSE;

		UINT renderTargetBlendDescriptorIndex = 0;

		while (renderTargetBlendDescriptorIndex < descriptor.colorBlendState.attachments.count()) {
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
		graphicsPipelineStateDescriptor.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		graphicsPipelineStateDescriptor.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		graphicsPipelineStateDescriptor.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		graphicsPipelineStateDescriptor.RasterizerState.DepthClipEnable = TRUE;

		// TODO: Set correct multisample state (depends on the render pass).
		graphicsPipelineStateDescriptor.RasterizerState.MultisampleEnable = FALSE;

		graphicsPipelineStateDescriptor.RasterizerState.AntialiasedLineEnable = FALSE;
		graphicsPipelineStateDescriptor.RasterizerState.ForcedSampleCount = 0;
		graphicsPipelineStateDescriptor.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		graphicsPipelineStateDescriptor.DepthStencilState.DepthEnable = descriptor.depthStencilState.depthTest;
		graphicsPipelineStateDescriptor.DepthStencilState.DepthWriteMask = static_cast<D3D12_DEPTH_WRITE_MASK>(descriptor.depthStencilState.depthWrite);
		graphicsPipelineStateDescriptor.DepthStencilState.DepthFunc = static_cast<D3D12_COMPARISON_FUNC>(static_cast<uint8>(descriptor.depthStencilState.compareOperation) + 1);
		graphicsPipelineStateDescriptor.DepthStencilState.StencilEnable = FALSE;
		graphicsPipelineStateDescriptor.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		graphicsPipelineStateDescriptor.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		graphicsPipelineStateDescriptor.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		graphicsPipelineStateDescriptor.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		graphicsPipelineStateDescriptor.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		graphicsPipelineStateDescriptor.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		graphicsPipelineStateDescriptor.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		graphicsPipelineStateDescriptor.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		graphicsPipelineStateDescriptor.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		graphicsPipelineStateDescriptor.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		UINT inputElementDescriptorIndex = 0;

		for (const VertexInputBinding& vertexInputBinding : descriptor.vertexInputBindings) {
			inputElementDescriptorIndex += static_cast<UINT>(vertexInputBinding.getAttributes().size());
		}

		sequence<D3D12_INPUT_ELEMENT_DESC> inputElementDescriptors(inputElementDescriptorIndex);
		UINT vertexInputBindingIndex = 0;

		inputElementDescriptorIndex = 0;

		while (vertexInputBindingIndex < descriptor.vertexInputBindings.count()) {
			const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[vertexInputBindingIndex];
			const std::vector<VertexInputAttribute>& vertexInputAttributes = vertexInputBinding.getAttributes();

			for (const VertexInputAttribute& vertexInputAttribute : vertexInputAttributes) {
				D3D12_INPUT_ELEMENT_DESC& inputElementDescriptor = inputElementDescriptors[inputElementDescriptorIndex];
				inputElementDescriptor.SemanticName = "VERTEX_INPUT_ATTRIBUTE";
				inputElementDescriptor.SemanticIndex = inputElementDescriptorIndex;
				inputElementDescriptor.Format = getVertexInputAttributeType(vertexInputAttribute.type);
				inputElementDescriptor.InputSlot = vertexInputBindingIndex;
				inputElementDescriptor.AlignedByteOffset = vertexInputAttribute.offset;

				if (vertexInputBinding.getInputRate() == InputRate::INSTANCE) {
					inputElementDescriptor.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
					inputElementDescriptor.InstanceDataStepRate = 1;
				}
				else {
					inputElementDescriptor.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
					inputElementDescriptor.InstanceDataStepRate = 0;
				}

				inputElementDescriptorIndex += 1;
			}

			vertexInputBindingIndex += 1;
		}

		graphicsPipelineStateDescriptor.InputLayout.pInputElementDescs = &inputElementDescriptors[0];
		graphicsPipelineStateDescriptor.InputLayout.NumElements = static_cast<UINT>(inputElementDescriptors.count());
		graphicsPipelineStateDescriptor.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		graphicsPipelineStateDescriptor.PrimitiveTopologyType = getPrimitiveTopologyType(descriptor.primitiveTopology);
		graphicsPipelineStateDescriptor.NumRenderTargets = subpass->getColorAttachmentReferenceCount();

		const view<AttachmentReference> colorAttachmentReferences = subpass->getColorAttachmentReferences();

		UINT renderTargetIndex = 0;

		for (const AttachmentReference& colorAttachmentReference : subpass->getColorAttachmentReferences()) {
			if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
				continue;
			}

			const Attachment& colorAttachment = descriptor.renderPass->getAttachment(colorAttachmentReference.index);

			graphicsPipelineStateDescriptor.RTVFormats[renderTargetIndex] = getImageFormat(colorAttachment.format);

			renderTargetIndex += 1;
		}

		if (subpass->hasDepthStencilAttachmentReference()) {
			ADAPTER_ASSERT(graphicsPipelineStateDescriptor.DSVFormat == DXGI_FORMAT_UNKNOWN, "Depth stencil format is already provided.");

			const AttachmentReference& depthStencilAttachmentReference = subpass->getDepthStencilAttachmentReference();
			const Attachment& depthStencilAttachment = descriptor.renderPass->getAttachment(depthStencilAttachmentReference.index);

			graphicsPipelineStateDescriptor.DSVFormat = getImageFormat(depthStencilAttachment.format);
		}

		graphicsPipelineStateDescriptor.SampleDesc.Count = static_cast<UINT>(subpass->getRasterizationSampleCount());
		graphicsPipelineStateDescriptor.SampleDesc.Quality = 0;
		graphicsPipelineStateDescriptor.NodeMask = 0;
		graphicsPipelineStateDescriptor.CachedPSO.pCachedBlob = nullptr;
		graphicsPipelineStateDescriptor.CachedPSO.CachedBlobSizeInBytes = 0;
		graphicsPipelineStateDescriptor.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		result = device.device->CreateGraphicsPipelineState(&graphicsPipelineStateDescriptor, IID_PPV_ARGS(&pipelineState));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create graphics pipeline state.");

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				const LPWSTR name = ConvertUTF8ToUTF16(&descriptor.name[0]);

				result = pipelineState->SetName(name);

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set pipeline state name.");

				free(name);
			}
		#endif
	}

	DX12::RenderPipeline::~RenderPipeline() {
		pipelineState->Release();
	}
}