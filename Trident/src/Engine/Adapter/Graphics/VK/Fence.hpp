// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Graphics/Fence.hpp"

namespace Trident {
	class VK::Fence final : public Trident::Fence {
		public:
			explicit Fence(const Trident::Device* device);

		public:
			VkFence fence;
	};

	namespace VK {
		ref<Trident::Fence> createFence(const Trident::Device* device, const FenceDescriptor& descriptor);

		void destroyFence(const Trident::Device* device, const Trident::Fence* fence);
	}
}