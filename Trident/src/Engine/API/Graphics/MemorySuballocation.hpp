// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct MemorySuballocation {
		/// The index of the parent memory allocation.
		uint32 allocationIndex;

		/// The memory properties for this suballocation.
		/// These can differ slightly from the allocation's memory type,
		/// depending on the usage of the resource.
		MemoryProperty memoryProperties;

		uint64 offset;
		uint64 size;
	};
}