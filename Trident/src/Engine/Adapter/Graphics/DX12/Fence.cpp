// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Fence.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::Fence::Fence(const Trident::Device* device) :
		Trident::Fence(device)
	{}

	ref<Trident::Fence> DX12::createFence(const Trident::Device* device, const FenceDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		ref<DX12::Fence> fence = createRef<DX12::Fence>(device);
		HRESULT result;

		result = _device->device->CreateFence(descriptor.signaled, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence->fence));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create fence.");

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				const LPWSTR name = ConvertUTF8ToUTF16(&descriptor.name[0]);

				result = fence->fence->SetName(name);

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set fence name.");

				free(name);
			}
		#endif

		return fence;
	}

	void DX12::destroyFence(const Trident::Device* device, const Trident::Fence* fence) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Fence* _fence = static_cast<const DX12::Fence*>(fence);

		_fence->fence->Release();
	}
}