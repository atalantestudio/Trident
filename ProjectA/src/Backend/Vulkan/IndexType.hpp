#pragma once

namespace ProjectA {
	// See https://registry.khronos.org/vulkan/specs/latest/man/html/VkIndexType.html.
	// TODO: Support VK_INDEX_TYPE_NONE_KHR and VK_INDEX_TYPE_UINT8_KHR?
	static VkIndexType getVkIndexType(uint8 indexTypeSize) {
		switch (indexTypeSize) {
			case sizeof(uint16):
				return VK_INDEX_TYPE_UINT16;
			case sizeof(uint32):
				return VK_INDEX_TYPE_UINT32;
			default:
				ENGINE_ASSERT(false, "Unsupported index type size.");

				return VK_INDEX_TYPE_MAX_ENUM;
		}
	}
}