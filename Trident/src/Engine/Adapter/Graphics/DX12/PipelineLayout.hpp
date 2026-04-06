// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/API/Graphics/PipelineLayout.hpp"

namespace Trident {
	class DX12::PipelineLayout final : public Trident::PipelineLayout {
		public:
			explicit PipelineLayout(const Trident::Device* device);

		public:
			ID3D12RootSignature* rootSignature;
			UINT samplerDescriptorTableRootParameterIndex = max<UINT>();
	};

	namespace DX12 {
		ref<Trident::PipelineLayout> createPipelineLayout(const Trident::Device* device, const PipelineLayoutDescriptor& descriptor);

		void destroyPipelineLayout(const Trident::Device* device, const Trident::PipelineLayout* pipelineLayout);
	}
}