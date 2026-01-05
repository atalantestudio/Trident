// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"

#include "Engine/API/Rendering/Fence.hpp"

namespace ProjectA {
	class DX12::Fence final : public ProjectA::Fence {
		public:
			explicit Fence(const DX12::Device& device, bool signaled);

		public:
			ComPtr<ID3D12Fence> fence;
	};
}