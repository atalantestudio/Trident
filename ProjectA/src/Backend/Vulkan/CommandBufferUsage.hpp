#pragma once

namespace ProjectA {
	static VkCommandBufferUsageFlags getVkCommandBufferUsageFlag(CommandBufferUsage commandBufferUsage) {
		return static_cast<VkCommandBufferUsageFlags>(commandBufferUsage);
	}
}