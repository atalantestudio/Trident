#pragma once

#include "Engine/API/Rendering/CommandBufferUsage.hpp"

namespace ProjectA {
	static VkCommandBufferUsageFlags getVkCommandBufferUsageFlag(CommandBufferUsage commandBufferUsage) {
		return static_cast<VkCommandBufferUsageFlags>(commandBufferUsage);
	}
}