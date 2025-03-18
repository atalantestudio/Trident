#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/Buffer/BufferUsage.hpp"

namespace ProjectA {
	static VkBufferUsageFlagBits getVkBufferUsageFlagBits(BufferUsage bufferUsage) {
		static constexpr VkBufferUsageFlagBits vkBufferUsageFlagBits[] = {
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
		};

		return vkBufferUsageFlagBits[static_cast<sint32>(bufferUsage)];
	}
}