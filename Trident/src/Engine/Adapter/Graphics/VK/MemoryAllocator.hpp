// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/MemoryAllocator.hpp"
#include "Engine/API/Graphics/MemoryRequirements.hpp"
#include "Engine/API/Graphics/PhysicalDevice.hpp"

namespace Trident {
	class VK::MemoryAllocator : public Trident::MemoryAllocator {
		public:
			explicit MemoryAllocator(const Trident::Device* device);

			#if defined(DEBUG_ENUMERATE_MEMORY_HEAPS)
				void enumerateMemoryHeaps();
			#endif

			#if defined(DEBUG_ENUMERATE_MEMORY_TYPES)
				void enumerateMemoryTypes();
			#endif

			/// Finds and returns the index of a suitable memory type for the provided memory requirements, or ~0 if none was found.
			/// Memory types having exactly the same flags as the requirements are prioritized.
			uint32 findMemoryTypeIndex(uint32 memoryTypeBits, VkMemoryPropertyFlags memoryPropertyFlags);

			void* mapSuballocation(const MemorySuballocation& memorySuballocation, VkDeviceSize offset, VkDeviceSize size);

			void unmapSuballocation(uint32 memoryAllocationIndex);

			void freeSuballocation(uint32 memoryAllocationIndex);
	};

	namespace VK {
		MemoryAllocation allocateMemory(Trident::MemoryAllocator* memoryAllocator, bool dynamic, uint32 memoryTypeIndex, uint64 size, uint64 alignment);

		uint32 findMemoryTypeIndex(const Trident::MemoryAllocator* memoryAllocator, MemoryProperty memoryProperties);

		MemoryRequirements getBufferMemoryRequirements(const Trident::MemoryAllocator* memoryAllocator, ref<Trident::BaseBuffer> buffer);

		MemoryRequirements getImageMemoryRequirements(const Trident::MemoryAllocator* memoryAllocator, ref<Trident::Image> image);

		MemorySuballocation suballocateBufferMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Trident::BaseBuffer> buffer);

		MemorySuballocation suballocateImageMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Trident::Image> image);
	}
}