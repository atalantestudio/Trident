#pragma once

namespace ProjectA {
	static VkCommandBufferUsageFlags getVkCommandBufferUsageFlag(CommandBufferUsage commandBufferUsage) {
		static constexpr VkCommandBufferUsageFlags vkCommandBufferUsageFlags[] = {
			0,
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		};

		return vkCommandBufferUsageFlags[static_cast<uint8>(commandBufferUsage)];
	}
}