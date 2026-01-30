// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/API/Rendering/ComputePipeline.hpp"

namespace Trident {
	class DX12::ComputePipeline final : public Trident::ComputePipeline {
		public:
			explicit ComputePipeline(const ComputePipelineDescriptor& descriptor, const DX12::Device& device);

			~ComputePipeline() final override;

		public:
			ID3D12PipelineState* pipelineState;
			ref<DX12::PipelineLayout> pipelineLayout;
	};
}