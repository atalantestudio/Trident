// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class BufferUsage : uint16 {
		TRANSFER_SRC = BIT(0),
		TRANSFER_DST = BIT(1),
		UNIFORM = BIT(2),

		// The buffer will only be read by the device.
		READ_ONLY_STORAGE = BIT(3),

		// The buffer may be written to by the device.
		READ_WRITE_STORAGE = BIT(4),

		INDEX = BIT(5),
		VERTEX = BIT(6),
		INDIRECT = BIT(7),
	};

	AS_FLAG(BufferUsage);
}