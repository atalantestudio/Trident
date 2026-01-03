// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/CommandPool.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::CommandPool::CommandPool(const DX12::Device& device, D3D12_COMMAND_LIST_TYPE type) :
		type(type)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const HRESULT result = device.device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command allocator.");
	}
}