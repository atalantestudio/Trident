// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Backend/DX12/Device.hpp"

#include "Engine/API/Rendering/PipelineLayout.hpp"

namespace ProjectA {
	class DX12::PipelineLayout : public ProjectA::PipelineLayout {
		public:
			explicit PipelineLayout(const PipelineLayoutDescriptor& descriptor, const DX12::Device& device);

			~PipelineLayout() final override;

		private:
			ComPtr<ID3DBlob> rootSignature;
	};
}