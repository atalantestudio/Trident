// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/API/Rendering/RenderPipeline.hpp"

namespace Trident {
	class DX12::RenderPipeline final : public Trident::RenderPipeline {
		public:
			explicit RenderPipeline(const RenderPipelineDescriptor& descriptor, const DX12::Device& device);

		public:
			ref<DX12::PipelineLayout> pipelineLayout;
			ComPtr<ID3D12PipelineState> pipelineState;
			D3D_PRIMITIVE_TOPOLOGY primitiveTopology;
	};
}