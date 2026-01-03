#pragma once

#include "Engine/API/Rendering/BufferUsage.hpp"

namespace ProjectA {
	static VkBufferUsageFlagBits getVkBufferUsageFlagBits(BufferUsage bufferUsage) {
		return static_cast<VkBufferUsageFlagBits>(bufferUsage);
	}
}