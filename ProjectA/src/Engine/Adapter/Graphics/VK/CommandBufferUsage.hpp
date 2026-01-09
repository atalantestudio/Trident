// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/CommandBufferUsage.hpp"

namespace ProjectA::VK {
	inline VkCommandBufferUsageFlags getCommandBufferUsageFlag(CommandBufferUsage commandBufferUsage) {
		return static_cast<VkCommandBufferUsageFlags>(commandBufferUsage);
	}
}