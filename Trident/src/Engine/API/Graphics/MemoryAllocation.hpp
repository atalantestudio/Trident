// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/MemoryProperty.hpp"

namespace Trident {
	struct MemoryAllocation {
		/// Whether the allocation was made on-demand or for an explicit user request.
		bool dynamic;

		/// Opaque memory object handle.
		uint64 memory;

		// TODO(graphics/memory-allocation): Also include memory property flags?
		uint32 memoryTypeIndex;
		MemoryProperty memoryProperties;

		/// Size of the allocation, in bytes.
		uint64 size;

		/// Size of the used partition, in bytes.
		uint64 usedOffset;

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