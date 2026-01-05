// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/CommandBuffer.hpp"

namespace ProjectA {
	DX12::CommandBuffer::CommandBuffer(ComPtr<ID3D12GraphicsCommandList> commandList, uint32 commandCount) :
		ProjectA::CommandBuffer(commandCount),
		commandList(commandList)
	{}
}