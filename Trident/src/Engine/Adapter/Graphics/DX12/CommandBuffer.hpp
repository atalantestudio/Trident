// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/CommandBuffer.hpp"

namespace Trident {
	class DX12::CommandBuffer final : public Trident::CommandBuffer {
		public:
			explicit CommandBuffer(ID3D12GraphicsCommandList* commandList, uint32 commandCount);

		public:
			ID3D12GraphicsCommandList* commandList;
	};
}