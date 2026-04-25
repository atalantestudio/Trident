// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// TODO: Use.
	struct MemoryAllocation {
		uint64 offset;
		uint64 size;

		/// Index of the memory type, used when mapping suballocations.
		uint32 memoryTypeIndex;

		/// Number of active suballocations.
		uint32 suballocationCount;

		/// Alignment of the most recent suballocation in this allocation.
		uint64 lastSuballocationAlignment;

		/// Number of suballocations that are in the mapped state.
		uint32 mappedSuballocationCount;

		/// Cached mapped range for multiple mapped suballocations in the same allocation.
		void* mappedRange;
	};
}