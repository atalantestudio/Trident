#pragma once

namespace Trident {
	enum class BufferUsage : uint16 {
		TRANSFER_SRC = BIT(0), // 0001
		TRANSFER_DST = BIT(1), // 0010

		// NOTE: BIT(2) and BIT(3) are omitted to allow easy casting to VkBufferUsageFlagBits.

		UNIFORM = BIT(4),
		STORAGE = BIT(5),
		INDEX = BIT(6),
		VERTEX = BIT(7),
		INDIRECT = BIT(8),
	};

	AS_FLAG(BufferUsage);
}