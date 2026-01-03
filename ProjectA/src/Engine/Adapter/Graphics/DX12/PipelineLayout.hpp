// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"

#include "Engine/API/Rendering/PipelineLayout.hpp"

namespace ProjectA {
	class DX12::PipelineLayout : public ProjectA::PipelineLayout {
		public:
			explicit PipelineLayout(const PipelineLayoutDescriptor& descriptor, const DX12::Device& device);

			~PipelineLayout() final override;

		public:
			ComPtr<ID3D12RootSignature> rootSignature;
	};
}