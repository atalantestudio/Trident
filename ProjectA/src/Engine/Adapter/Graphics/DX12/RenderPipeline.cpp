// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/CullMode.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/PrimitiveTopology.hpp"
#include "Engine/Adapter/Graphics/DX12/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/DX12/VertexInputAttributeType.hpp"

namespace ProjectA {
	DX12::RenderPipeline::RenderPipeline(const RenderPipelineDescriptor& descriptor, const DX12::Device& device)
		// : ProjectA::RenderPipeline(descriptor)
	{
		const ref<DX12::PipelineLayout> pipelineLayout = std::static_pointer_cast<DX12::PipelineLayout>(descriptor.pipelineLayout);
		const ref<DX12::ShaderModule> vertexShaderModule = std::static_pointer_cast<DX12::ShaderModule>(descriptor.vertexShaderModule);
		const ref<DX12::ShaderModule> fragmentShaderModule = std::static_pointer_cast<DX12::ShaderModule>(descriptor.fragmentShaderModule);

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

		// TODO: Set correct blend state.
		graphicsPipelineStateDescriptor.BlendState.AlphaToCoverageEnable = FALSE;
		graphicsPipelineStateDescriptor.BlendState.IndependentBlendEnable = FALSE;
		graphicsPipelineStateDescriptor.BlendState.RenderTarget[0].BlendEnable = FALSE;
		graphicsPipelineStateDescriptor.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
		graphicsPipelineStateDescriptor.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		graphicsPipelineStateDescriptor.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		graphicsPipelineStateDescriptor.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		graphicsPipelineStateDescriptor.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		graphicsPipelineStateDescriptor.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		graphicsPipelineStateDescriptor.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		graphicsPipelineStateDescriptor.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
		graphicsPipelineStateDescriptor.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		graphicsPipelineStateDescriptor.SampleMask = max<UINT>();

		graphicsPipelineStateDescriptor.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		graphicsPipelineStateDescriptor.RasterizerState.CullMode = getCullMode(descriptor.cullMode);
		graphicsPipelineStateDescriptor.RasterizerState.FrontCounterClockwise = descriptor.frontFace == FrontFace::COUNTER_CLOCKWISE;
		graphicsPipelineStateDescriptor.RasterizerState.DepthBias = 0;
		graphicsPipelineStateDescriptor.RasterizerState.DepthBiasClamp = 0.0f;
		graphicsPipelineStateDescriptor.RasterizerState.SlopeScaledDepthBias = 0.0f;
		graphicsPipelineStateDescriptor.RasterizerState.DepthClipEnable = FALSE;

		// TODO: Set correct multisample state.
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
				inputElementDescriptor.SemanticName = nullptr;
				inputElementDescriptor.SemanticIndex = max<UINT>();
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

		// TODO: Set correct render targets.
		graphicsPipelineStateDescriptor.NumRenderTargets = 1;
		graphicsPipelineStateDescriptor.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		graphicsPipelineStateDescriptor.DSVFormat = DXGI_FORMAT_UNKNOWN;

		// TODO: Set correct sample count.
		graphicsPipelineStateDescriptor.SampleDesc.Count = 1;

		graphicsPipelineStateDescriptor.SampleDesc.Quality = 0;
		graphicsPipelineStateDescriptor.NodeMask = 0;
		graphicsPipelineStateDescriptor.CachedPSO.pCachedBlob = nullptr;
		graphicsPipelineStateDescriptor.CachedPSO.CachedBlobSizeInBytes = 0;
		graphicsPipelineStateDescriptor.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		BREAKPOINT();
	}
}