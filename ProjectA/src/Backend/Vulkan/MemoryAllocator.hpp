#pragma once

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

namespace ProjectA {
	#define UNDEFINED_MEMORY_TYPE_INDEX std::numeric_limits<uint32>::max()
	#define UNDEFINED_MEMORY_BLOCK_INDEX std::numeric_limits<uint32>::max()

	struct MemoryBlock {
		VkDeviceMemory memory;
		VkDeviceSize offset;
		VkDeviceSize size;
		uint32 memoryTypeIndex;
		uint32 suballocationCount;
		uint32 mappedSuballocationCount;
		void* mappedRange;
	};

	struct MemorySuballocation {
		uint32 blockIndex;
		VkDeviceSize offset;
		VkDeviceSize size;
	};

	class MemoryAllocator {
		public:
			static void initialize(const VulkanPhysicalDevice& physicalDevice);

			static void destroy();

			// Finds a suitable memory type for the requirements, and returns its index.
			static uint32 findMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags);

			static MemorySuballocation suballocateBuffer(VkDevice device, VkBuffer buffer, uint32 memoryTypeIndex, VkDeviceSize size);

			static MemorySuballocation suballocateImage(VkDevice device, VkImage image, uint32 memoryTypeIndex, VkDeviceSize size);

			static void* mapSuballocation(VkDevice device, const MemorySuballocation& memorySuballocation, VkDeviceSize size, VkDeviceSize offset);

			static void unmapSuballocation(VkDevice device, uint32 memoryBlockIndex);

			static void freeSuballocation(VkDevice device, uint32 memoryBlockIndex);

		private:
			// Allocates a new memory block.
			static void createAllocation(VkDevice device, uint32 memoryTypeIndex);

			// Creates and returns a suballocation in a memory block.
			static MemorySuballocation suballocate(VkDevice device, uint32 memoryTypeIndex, VkDeviceSize size);

		private:
			static constexpr VkDeviceSize MAX_MEMORY_BLOCK_SIZE = 2200;

		private:
			static const VulkanPhysicalDevice* physicalDevice;

			// Index of the currently allocated memory block for every memory type.
			static std::vector<uint32> currentMemoryBlockIndices;

			// Allocated memories from all memory types.
			static std::vector<MemoryBlock> memoryBlocks;
	};
}