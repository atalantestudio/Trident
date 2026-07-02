// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/MemoryAllocation.hpp"
#include "Engine/API/Graphics/MemoryRequirements.hpp"
#include "Engine/API/Graphics/MemorySuballocation.hpp"

namespace Trident {
	class Buffer;
	class Image;
	class Device;

	class MemoryAllocator {
		public:
			/// The default memory allocation size.
			/// An allocation can be larger than that if a large buffer or image
			/// was suballocated dynamically and didn't fit any of the existing allocations.
			static constexpr uint64 MIN_DYNAMIC_MEMORY_ALLOCATION_SIZE = 16000000;

		public:
			/// Creates a memory allocation and returns its index in the allocation list.
			uint32 allocate(bool dynamic, uint32 memoryTypeIndex, uint64 size, uint64 alignment);

			/// Finds and returns the memory type index corresponding to the provided memory property flag.
			uint32 findMemoryTypeIndex(MemoryProperty memoryProperties, bool transferDestination) const;

			#if defined(DEBUG_COUNT_ALLOCATED_MEMORY_SIZE)
				/// Returns the total size of the allocated memory.
				uint64 getAllocatedMemorySize() const;
			#endif

			/// Returns the memory requirements for the provided buffer or image.
			MemoryRequirements getMemoryRequirements(ref<Buffer> buffer) const;
			MemoryRequirements getMemoryRequirements(ref<Image> image) const;

			// TODO: Make private.
			/// Suballocates a memory allocation to fit the provided size, buffer or image.
			MemorySuballocation suballocate(uint32 allocationIndex, uint64 size, uint64 alignment);

			void suballocateStatic(uint32 allocationIndex, ref<Buffer> buffer);
			void suballocateStatic(uint32 allocationIndex, ref<Image> image);

			void suballocateDynamic(ref<Buffer> buffer);
			void suballocateDynamic(ref<Image> image);

			// TODO(graphics/memory-allocator/map-memory)
			// TODO(graphics/memory-allocator/unmap-memory)
			// TODO(graphics/memory-allocator/free-memory)

		private:
			/// Finds and returns the index of the first memory allocation that can contain the provided size.
			/// Returns the allocation count if no suitable allocation was found.
			uint32 findAllocationIndex(uint32 memoryTypeIndex, const MemoryRequirements& memoryRequirements) const;

		protected:
			explicit MemoryAllocator(const Device* device);

		// TODO: Make private.
		public:
			const Device* device;
			std::vector<MemoryAllocation> allocations;
			std::vector<uint32> freeableAllocationIndices;

			#if defined(DEBUG_COUNT_ALLOCATED_MEMORY_SIZE)
				uint64 allocatedMemorySize = 0;
			#endif
	};
}