// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Fence.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::Fence::Fence(const FenceDescriptor& descriptor, const DX12::Device& device) :
		Trident::Fence(descriptor)
	{
		HRESULT result;

		result = device.device->CreateFence(descriptor.signaled, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create fence.");
	}

	DX12::Fence::~Fence() {
		fence->Release();
	}
}