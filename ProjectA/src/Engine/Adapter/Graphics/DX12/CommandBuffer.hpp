// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/CommandBuffer.hpp"

namespace ProjectA {
	class DX12::CommandBuffer final : public ProjectA::CommandBuffer {
		public:
			explicit CommandBuffer(ComPtr<ID3D12GraphicsCommandList> commandList, uint32 commandCount);

		public:
			ComPtr<ID3D12GraphicsCommandList> commandList;
	};
}