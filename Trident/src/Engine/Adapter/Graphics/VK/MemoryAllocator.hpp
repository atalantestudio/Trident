// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/MemoryAllocator.hpp"
#include "Engine/API/Graphics/PhysicalDevice.hpp"

namespace Trident {
	#define UNDEFINED_MEMORY_TYPE_INDEX max<uint32>()
	#define UNDEFINED_MEMORY_ALLOCATION_INDEX max<uint32>()

	#define DEBUG_ENUMERATE_HEAPS
	#define DEBUG_ENUMERATE_TYPES

	class VK::MemoryAllocator : public Trident::MemoryAllocator {
		private:
			// Memory allocations have a default size of 16Mb.
			static constexpr VkDeviceSize DEFAULT_MEMORY_ALLOCATION_SIZE = 16000000;

		public:
			explicit MemoryAllocator(const Trident::Device* device);

			#if defined(BUILD_MODE_DEBUG)
				void enumerateMemoryHeaps();

				void enumerateMemoryTypes();
			#endif

			/// Finds and returns the index of a suitable memory type for the provided memory requirements,
			/// or UNDEFINED_MEMORY_TYPE_INDEX if none was found.
			/// Memory types having exactly the same flags as the requirements are prioritized.
			/// Alignment and available size are ignored.
			uint32 findSuitableMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags);

			MemorySuballocation suballocateBuffer(VkBuffer buffer, uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements);

			MemorySuballocation suballocateImage(VkImage image, uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements);

			void* mapSuballocation(const MemorySuballocation& memorySuballocation, VkDeviceSize offset, VkDeviceSize size);

			void unmapSuballocation(uint32 memoryAllocationIndex);

			void freeSuballocation(uint32 memoryAllocationIndex);

		private:
			// Creates and returns a suballocation from a memory allocation.
			MemorySuballocation suballocate(uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements, sequence<uint32>& currentMemoryAllocationIndices);

		// TODO: Don't expose.
		public:
			const VK::PhysicalDevice* physicalDevice;

			// TODO: Try merging with currentImageMemoryAllocationIndices.
			// Indices of the currently active buffer memory allocations per memory type.
			sequence<uint32> currentBufferMemoryAllocationIndices;

			// Indices of the currently active image memory allocations per memory type.
			sequence<uint32> currentImageMemoryAllocationIndices;

			std::vector<uint32> freeMemoryAllocationIndices;
	};

	namespace VK {
		uint32 createMemoryAllocation(Trident::MemoryAllocator* memoryAllocator, uint32 memoryTypeIndex, uint64 size);
	}
}