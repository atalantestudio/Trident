// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/BufferUsage.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::VK {
	inline VkBufferUsageFlagBits getBufferUsageFlagBits(BufferUsage bufferUsage) {
		VkBufferUsageFlags bufferUsageFlags{};

		if (bufferUsage & BufferUsage::TRANSFER_SRC) {
			bufferUsageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		}

		if (bufferUsage & BufferUsage::TRANSFER_DST) {
			bufferUsageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}

		if (bufferUsage & BufferUsage::UNIFORM) {
			bufferUsageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}

		if (bufferUsage & BufferUsage::READ_ONLY_STORAGE) {
			bufferUsageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}

		if (bufferUsage & BufferUsage::READ_WRITE_STORAGE) {
			bufferUsageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}

		if (bufferUsage & BufferUsage::INDEX) {
			bufferUsageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}

		if (bufferUsage & BufferUsage::VERTEX) {
			bufferUsageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}

		if (bufferUsage & BufferUsage::INDIRECT) {
			bufferUsageFlags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}

		if (bufferUsage & BufferUsage::SHADER_DEVICE_ADDRESS) {
			bufferUsageFlags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		}

		if (bufferUsage & BufferUsage::ACCELERATION_STRUCTURE_STORAGE) {
			bufferUsageFlags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
		}

		if (bufferUsage & BufferUsage::READ_ONLY_ACCELERATION_STRUCTURE_INPUT) {
			bufferUsageFlags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
		}

		ADAPTER_ASSERT(bufferUsageFlags != 0, "Unknown buffer usage flag combination.");

		return static_cast<VkBufferUsageFlagBits>(bufferUsageFlags);
	}
}