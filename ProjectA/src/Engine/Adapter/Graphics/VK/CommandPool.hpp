// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/CommandPool.hpp"

namespace ProjectA {
	class VK::CommandPool final : public ProjectA::CommandPool {
		public:
			explicit CommandPool(const VK::Device& device, uint32 queueFamilyIndex);

			~CommandPool() final override;

		public:
			VkCommandPool commandPool;

		private:
			const VK::Device& device;
	};
}