// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Fence.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::Fence::Fence(const DX12::Device& device, bool signaled) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const HRESULT result = device.device->CreateFence(signaled, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create fence.");
	}
}