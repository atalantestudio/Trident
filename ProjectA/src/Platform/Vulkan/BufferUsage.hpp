#pragma once

#include "pch.hpp"

#include <vulkan/vulkan.h>

namespace ProjectA {
	static inline VkBufferUsageFlagBits getVkBufferUsageFlagBits(const uint32 bufferUsage) {
		static constexpr VkBufferUsageFlagBits vkBufferUsageFlagBits[] = {
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
		};

		return vkBufferUsageFlagBits[bufferUsage];
	}
}