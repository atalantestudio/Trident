// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/CommandPool.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::CommandPool::CommandPool(const DX12::Device& device, D3D12_COMMAND_LIST_TYPE type) :
		type(type)
	{
		HRESULT result;

		result = device.device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command allocator.");
	}

	DX12::CommandPool::~CommandPool() {
		commandAllocator->Release();
	}
}