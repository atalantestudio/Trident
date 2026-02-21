// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Fence.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::Fence::Fence(const FenceDescriptor& descriptor, const DX12::Device* device) :
		Trident::Fence(descriptor)
	{
		HRESULT result;

		result = device->device->CreateFence(descriptor.signaled, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create fence.");

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				const LPWSTR name = ConvertUTF8ToUTF16(&descriptor.name[0]);

				result = fence->SetName(name);

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set fence name.");

				free(name);
			}
		#endif
	}

	DX12::Fence::~Fence() {
		fence->Release();
	}
}