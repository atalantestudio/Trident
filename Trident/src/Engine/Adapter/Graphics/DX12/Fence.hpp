// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Fence.hpp"

namespace Trident {
	class DX12::Fence final : public Trident::Fence {
		public:
			explicit Fence(const Trident::Device* device);

		public:
			ID3D12Fence* fence;
	};

	namespace DX12 {
		ref<Trident::Fence> createFence(const Trident::Device* device, const FenceDescriptor& descriptor);

		void destroyFence(const Trident::Device* device, const Trident::Fence* fence);
	}
}