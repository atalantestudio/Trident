// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/CommandListType.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandPool.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::CommandPool::CommandPool(const Trident::Device* device, D3D12_COMMAND_LIST_TYPE commandListType) :
		Trident::CommandPool(device),
		commandListType(commandListType)
	{}

	ref<Trident::CommandPool> DX12::createCommandPool(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) {
		const D3D12_COMMAND_LIST_TYPE commandListType = getCommandListType(queueFamilyCapability);
		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		ref<DX12::CommandPool> commandPool = allocateRef<DX12::CommandPool>(device, commandListType);
		HRESULT result;

		result = _device->device->CreateCommandAllocator(commandListType, IID_PPV_ARGS(&commandPool->commandAllocator));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command allocator.");

		return commandPool;
	}

	void DX12::destroyCommandPool(const Trident::Device* device, const Trident::CommandPool* commandPool) {
		// The queue's command pool is destroyed by the device.
	}
}