#pragma once

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

namespace ProjectA {
	#define UNDEFINED_MEMORY_TYPE_INDEX max<uint32>()
	#define UNDEFINED_MEMORY_ALLOCATION_INDEX max<uint32>()

	// #define DEBUG_ENUMERATE_HEAPS
	// #define DEBUG_ENUMERATE_TYPES

	struct MemoryAllocation {
		VkDeviceMemory memory;
		VkDeviceSize offset;
		VkDeviceSize size;

		// Index of the memory type. Used with memory mapping.
		uint32 memoryTypeIndex;

		// Number of active suballocations.
		uint32 suballocationCount;

		// Alignment of the most recent suballocation in this allocation.
		VkDeviceSize lastSuballocationAlignment;

		// Number of suballocations in the mapped state.
		uint32 mappedSuballocationCount;

		// Cached mapped range for multiple mapped suballocations in the same allocation.
		void* mappedRange;
	};

	struct MemorySuballocation {
		uint32 allocationIndex;
		VkDeviceSize offset;
		VkDeviceSize size;
	};

	class MemoryAllocator {
		public:
			static void initialize(const VulkanPhysicalDevice& physicalDevice);

			static void destroy();

			#ifdef BUILD_MODE_DEBUG
				static void enumerateMemoryHeaps();

				static void enumerateMemoryTypes();

				static str8 formatSize(uint64 size);
			#endif

			// Finds a suitable memory type for the requirements, and returns its index.
			static uint32 findSuitableMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags);

			static MemorySuballocation suballocateBuffer(VkDevice device, VkBuffer buffer, uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements);

			static MemorySuballocation suballocateImage(VkDevice device, VkImage image, uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements);

			static void* mapSuballocation(VkDevice device, const MemorySuballocation& memorySuballocation, VkDeviceSize offset, VkDeviceSize size);

			static void unmapSuballocation(VkDevice device, uint32 memoryAllocationIndex);

			static void freeSuballocation(VkDevice device, uint32 memoryAllocationIndex);

		private:
			// Creates a new memory allocation and returns its index.
			static uint32 createAllocation(VkDevice device, uint32 memoryTypeIndex, VkDeviceSize size);

			// Creates and returns a suballocation from a memory allocation.
			// If isBuffer is false, the suballocation is done on image memory.
			static MemorySuballocation suballocate(VkDevice device, uint32 memoryTypeIndex, const VkMemoryRequirements& memoryRequirements, sequence<uint32>& currentMemoryAllocationIndices);

		private:
			// Memory allocations have a default size of 16Mb.
			static constexpr VkDeviceSize DEFAULT_MEMORY_ALLOCATION_SIZE = 16000000;

		private:
			static const VulkanPhysicalDevice* physicalDevice;

			// Allocated buffer and image memory allocations.
			static std::vector<MemoryAllocation> memoryAllocations;

			// TODO: Try Merging with currentImageMemoryAllocationIndices.
			// Indices of the currently active buffer memory allocations per memory type.
			static sequence<uint32> currentBufferMemoryAllocationIndices;

			// Indices of the currently active image memory allocations per memory type.
			static sequence<uint32> currentImageMemoryAllocationIndices;

			static std::vector<uint32> freeMemoryAllocationIndices;
	};
}