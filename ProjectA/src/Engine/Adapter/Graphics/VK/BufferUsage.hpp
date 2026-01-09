// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/BufferUsage.hpp"

namespace ProjectA::VK {
	inline VkBufferUsageFlagBits getBufferUsageFlagBits(BufferUsage bufferUsage) {
		return static_cast<VkBufferUsageFlagBits>(bufferUsage);
	}
}