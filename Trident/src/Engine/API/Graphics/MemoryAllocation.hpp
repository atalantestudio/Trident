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

		/// The index of the memory type hosting the allocation.
		uint32 memoryTypeIndex;

		/// Size of the allocation, in bytes.
		uint64 size;

		/// Size of the used partition, in bytes.
		uint64 usedOffset;

		/// Number of active suballocations.
		uint32 suballocationCount;

		/// Number of suballocations that are in the mapped state.
		uint32 mappedSuballocationCount;

		/// Cached mapped range for multiple mapped suballocations in the same allocation.
		void* mappedRange;

		// TODO(graphics/memory-allocation): Merge `dynamic` and `shaderDeviceAddress` in a flag enum.
		/// Whether the allocation contains buffers with the SHADER_DEVICE_ADDRESS flag.
		bool shaderDeviceAddress;
	};
}