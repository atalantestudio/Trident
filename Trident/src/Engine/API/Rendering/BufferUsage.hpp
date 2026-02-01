// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class BufferUsage : uint16 {
		TRANSFER_SRC = BIT(0), // 0001
		TRANSFER_DST = BIT(1), // 0010

		// NOTE: BIT(2) and BIT(3) are omitted to allow easy casting to VkBufferUsageFlagBits.

		UNIFORM = BIT(4),
		STORAGE = BIT(5),

		READ_ONLY_STORAGE = BIT(6),

		// The buffer may be written to by the device.
		READ_WRITE_STORAGE = BIT(7),

		INDEX = BIT(8),
		VERTEX = BIT(9),
		INDIRECT = BIT(10),
	};

	AS_FLAG(BufferUsage);
}