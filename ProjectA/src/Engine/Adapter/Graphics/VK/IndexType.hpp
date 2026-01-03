#pragma once

namespace ProjectA {
	static VkIndexType getVkIndexType(uint8 indexTypeSize) {
		switch (indexTypeSize) {
			case sizeof(uint16):
				return VK_INDEX_TYPE_UINT16;
			case sizeof(uint32):
				return VK_INDEX_TYPE_UINT32;
			default:
				ENGINE_ASSERT(false, "Unsupported index type size.");

				return static_cast<VkIndexType>(0);
		}
	}
}