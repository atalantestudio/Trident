// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct MemorySuballocation {
		/// The index of the parent memory allocation.
		uint32 allocationIndex;

		uint64 offset;
		uint64 size;
	};
}