// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/CommandPool.hpp"

namespace Trident {
	class VK::CommandPool final : public Trident::CommandPool {
		public:
			explicit CommandPool(const VK::Device& device, uint32 queueFamilyIndex);

			~CommandPool() final override;

		public:
			VkCommandPool commandPool;

		private:
			const VK::Device& device;
	};
}