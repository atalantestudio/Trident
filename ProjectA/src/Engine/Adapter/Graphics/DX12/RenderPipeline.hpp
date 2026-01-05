// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/API/Rendering/RenderPipeline.hpp"

namespace ProjectA {
	class DX12::RenderPipeline final : public ProjectA::RenderPipeline {
		public:
			explicit RenderPipeline(const RenderPipelineDescriptor& descriptor, const DX12::Device& device);

		private:
			ComPtr<ID3D12PipelineState> pipelineState;
	};
}