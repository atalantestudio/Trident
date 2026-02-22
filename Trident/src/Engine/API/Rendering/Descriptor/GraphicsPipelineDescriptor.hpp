// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ColorBlendAttachment.hpp"
#include "Engine/API/Rendering/CompareOperation.hpp"
#include "Engine/API/Rendering/CullMode.hpp"
#include "Engine/API/Rendering/DynamicState.hpp"
#include "Engine/API/Rendering/FrontFace.hpp"
#include "Engine/API/Rendering/PipelineLayout.hpp"
#include "Engine/API/Rendering/PolygonMode.hpp"
#include "Engine/API/Rendering/PrimitiveTopology.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"
#include "Engine/API/Rendering/VertexInputBinding.hpp"

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

		#if defined(BUILD_MODE_DEBUG)
			view<char8> name;
		#endif
	};
}