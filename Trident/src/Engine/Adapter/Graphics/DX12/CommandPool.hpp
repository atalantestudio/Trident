// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/API/Graphics/CommandPool.hpp"

namespace Trident {
	class DX12::CommandPool final : public Trident::CommandPool {
		public:
			explicit CommandPool(const Trident::Device* device, D3D12_COMMAND_LIST_TYPE commandListType);

		public:
			ID3D12CommandAllocator* commandAllocator;
			D3D12_COMMAND_LIST_TYPE commandListType;
	};

	namespace DX12 {
		ref<Trident::CommandPool> createCommandPool(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability);

		void destroyCommandPool(const Trident::Device* device, const Trident::CommandPool* commandPool);
	}
}