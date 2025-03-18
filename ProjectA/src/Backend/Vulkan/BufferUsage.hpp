#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/Buffer/BufferUsage.hpp"

namespace ProjectA {
	static VkBufferUsageFlagBits getVkBufferUsageFlagBits(BufferUsage bufferUsage) {
		return static_cast<VkBufferUsageFlagBits>(bufferUsage);
	}
}