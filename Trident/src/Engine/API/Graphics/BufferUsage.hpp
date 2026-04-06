// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class BufferUsage : uint8 {
		/// Indicates a buffer used as the source of a transfer command.
		TRANSFER_SRC = BIT(0),

		/// Indicates a buffer used as the destination of a transfer command.
		TRANSFER_DST = BIT(1),

		/// Indicates a uniform buffer.
		UNIFORM = BIT(2),

		/// Indicates a storage buffer that will only be read by the device.
		READ_ONLY_STORAGE = BIT(3),

		/// Indicates a storage buffer that will be read and/or written to by the device.
		READ_WRITE_STORAGE = BIT(4),

		/// Indicates an index buffer.
		INDEX = BIT(5),

		/// Indicates a vertex buffer.
		VERTEX = BIT(6),

		/// Indicates an indirect buffer.
		INDIRECT = BIT(7),
	};

	AS_FLAG(BufferUsage);
}