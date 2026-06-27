// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct MemoryRequirements {
		uint64 size;
		uint64 alignment;

		/// The packed indices of the supported memory types.
		uint32 memoryTypeBits;
	};
}