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
		public:
			explicit MemoryAllocator(const Trident::Device* device);

			#if defined(BUILD_MODE_DEBUG)
				void enumerateMemoryHeaps();

				void enumerateMemoryTypes();
			#endif

			/// Finds and returns the index of a suitable memory type for the provided memory requirements,
			/// or UNDEFINED_MEMORY_TYPE_INDEX if none was found.
			/// Memory types having exactly the same flags as the requirements are prioritized.
			/// In the second variant, alignment and available size are ignored.
			uint32 findSuitableMemoryType(VkMemoryPropertyFlags memoryPropertyFlags);
			uint32 findSuitableMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags);

			MemorySuballocation suballocateBuffer(VkBuffer buffer, uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements);

			MemorySuballocation suballocateImage(VkImage image, uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements);

			void* mapSuballocation(const MemorySuballocation& memorySuballocation, VkDeviceSize offset, VkDeviceSize size);

			void unmapSuballocation(uint32 memoryAllocationIndex);

			void freeSuballocation(uint32 memoryAllocationIndex);

		private:
			uint32 findAllocation(uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements, sequence<uint32>& currentMemoryAllocationIndices);

			uint32 allocateMemoryDynamic(uint64 size, MemoryProperty memoryProperties);

		// TODO: Don't expose.
		public:
			const VK::PhysicalDevice* physicalDevice;

			// TODO: Try merging with currentImageMemoryAllocationIndices.
			// Indices of the currently active buffer memory allocations per memory type.
			sequence<uint32> currentBufferMemoryAllocationIndices;

			// Indices of the currently active image memory allocations per memory type.
			sequence<uint32> currentImageMemoryAllocationIndices;

			std::vector<uint32> freeMemoryAllocationIndices;

			// TODO(graphics/vk/memory-allocator): Refactor.
			bool bufferOrImageMemoryAllocationIndices = false;
	};

	namespace VK {
		uint32 allocateMemory(Trident::MemoryAllocator* memoryAllocator, uint64 size, MemoryProperty memoryProperties);

		MemorySuballocation suballocateMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, uint64 size, uint64 alignment);
	}
}