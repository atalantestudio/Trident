// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/CommandBufferUsage.hpp"

namespace Trident::VK {
	inline VkCommandBufferUsageFlags getCommandBufferUsageFlag(CommandBufferUsage commandBufferUsage) {
		static constexpr VkCommandBufferUsageFlags COMMAND_BUFFER_USAGE_FLAGS[] = {
			0,
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		};

		ADAPTER_ASSERT(commandBufferUsage < CommandBufferUsage::count, "Invalid command buffer usage.");

		return COMMAND_BUFFER_USAGE_FLAGS[static_cast<uint32>(commandBufferUsage)];
	}
}