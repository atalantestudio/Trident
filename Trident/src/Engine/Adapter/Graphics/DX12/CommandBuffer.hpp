// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/CommandBuffer.hpp"

namespace Trident {
	class DX12::CommandBuffer final : public Trident::CommandBuffer {
		public:
			explicit CommandBuffer(ComPtr<ID3D12GraphicsCommandList> commandList, uint32 commandCount);

		public:
			ComPtr<ID3D12GraphicsCommandList> commandList;
	};
}