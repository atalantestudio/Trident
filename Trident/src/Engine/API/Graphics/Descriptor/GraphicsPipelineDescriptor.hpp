// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ColorBlendAttachment.hpp"
#include "Engine/API/Graphics/CompareOperation.hpp"
#include "Engine/API/Graphics/CullMode.hpp"
#include "Engine/API/Graphics/DynamicState.hpp"
#include "Engine/API/Graphics/FrontFace.hpp"
#include "Engine/API/Graphics/PipelineLayout.hpp"
#include "Engine/API/Graphics/PolygonMode.hpp"
#include "Engine/API/Graphics/PrimitiveTopology.hpp"
#include "Engine/API/Graphics/RenderPass.hpp"
#include "Engine/API/Graphics/ShaderModule.hpp"
#include "Engine/API/Graphics/VertexInputBinding.hpp"

namespace Trident {
	struct ColorBlendState {
		view<ColorBlendAttachment> attachments;
	};

	struct DepthStencilState {
		bool depthTest;
		bool depthWrite;
		CompareOperation compareOperation;
	};

	struct GraphicsPipelineDescriptor {
		ref<PipelineLayout> pipelineLayout;
		sequence<VertexInputBinding> vertexInputBindings;
		ref<ShaderModule> vertexShaderModule;
		ref<ShaderModule> fragmentShaderModule;
		PrimitiveTopology primitiveTopology;
		PolygonMode polygonMode;
		CullMode cullMode;
		FrontFace frontFace;

		// NOTE: Not supported by the DX12 adapter.
		float32 lineWidth;

		float32 minSampleShading;
		ColorBlendState colorBlendState;
		DepthStencilState depthStencilState;
		DynamicState dynamicState;
		ref<RenderPass> renderPass;
		uint32 subpassIndex;

		#if defined(DEBUG_OBJECT_NAME)
			view<char8> name;
		#endif
	};
}