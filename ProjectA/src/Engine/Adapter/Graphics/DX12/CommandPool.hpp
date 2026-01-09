// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/API/Rendering/CommandPool.hpp"

namespace ProjectA {
	class DX12::CommandPool final : public ProjectA::CommandPool {
		public:
			explicit CommandPool(const DX12::Device& device, D3D12_COMMAND_LIST_TYPE type);

		public:
			ComPtr<ID3D12CommandAllocator> commandAllocator;
			D3D12_COMMAND_LIST_TYPE type;
	};
}