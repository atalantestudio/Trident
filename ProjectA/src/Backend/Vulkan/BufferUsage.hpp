#pragma once

namespace ProjectA {
	static VkBufferUsageFlagBits getVkBufferUsageFlagBits(BufferUsage bufferUsage) {
		return static_cast<VkBufferUsageFlagBits>(bufferUsage);
	}
}