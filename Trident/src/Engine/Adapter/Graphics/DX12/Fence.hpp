// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/API/Rendering/Fence.hpp"

namespace Trident {
	class DX12::Fence final : public Trident::Fence {
		public:
			explicit Fence(const FenceDescriptor& descriptor, const DX12::Device* device);

			~Fence() final override;

		public:
			ID3D12Fence* fence;
	};
}