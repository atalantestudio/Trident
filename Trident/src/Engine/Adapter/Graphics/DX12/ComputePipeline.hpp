// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/API/Rendering/ComputePipeline.hpp"

namespace Trident {
	class DX12::ComputePipeline final : public Trident::ComputePipeline {
		public:
			explicit ComputePipeline(const Trident::Device* device);

		public:
			ID3D12PipelineState* pipelineState;
			ref<DX12::PipelineLayout> pipelineLayout;
	};

	namespace DX12 {
		ref<Trident::ComputePipeline> createComputePipeline(const Trident::Device* device, const ComputePipelineDescriptor& descriptor);

		void destroyComputePipeline(const Trident::Device* device, const Trident::ComputePipeline* computePipeline);
	}
}