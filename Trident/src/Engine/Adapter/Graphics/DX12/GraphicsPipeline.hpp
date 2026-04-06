// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/API/Graphics/GraphicsPipeline.hpp"

namespace Trident {
	class DX12::GraphicsPipeline final : public Trident::GraphicsPipeline {
		public:
			explicit GraphicsPipeline(const Trident::Device* device);

		public:
			ID3D12PipelineState* pipelineState;
			ref<DX12::PipelineLayout> pipelineLayout;
			D3D_PRIMITIVE_TOPOLOGY primitiveTopology;
	};

	namespace DX12 {
		ref<Trident::GraphicsPipeline> createGraphicsPipeline(const Trident::Device* device, const GraphicsPipelineDescriptor& descriptor);

		void destroyGraphicsPipeline(const Trident::Device* device, const Trident::GraphicsPipeline* graphicsPipeline);
	}
}