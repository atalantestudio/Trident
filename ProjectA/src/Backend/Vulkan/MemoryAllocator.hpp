#pragma once

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

namespace ProjectA {
	#define UNDEFINED_MEMORY_TYPE_INDEX std::numeric_limits<uint32>::max()
	#define UNDEFINED_MEMORY_ALLOCATION_INDEX std::numeric_limits<uint32>::max()

	struct MemoryAllocation {
		VkDeviceMemory memory;
		VkDeviceSize offset;
		VkDeviceSize size;
		uint32 memoryTypeIndex;
		uint32 suballocationCount;
		uint32 mappedSuballocationCount;
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
				static void enumerateMemoryTypes();
			#endif

			// Finds a suitable memory type for the requirements, and returns its index.
			static uint32 findMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags);

			static MemorySuballocation suballocateBuffer(VkDevice device, VkBuffer buffer, uint32 memoryTypeIndex, VkDeviceSize size);

			static MemorySuballocation suballocateImage(VkDevice device, VkImage image, uint32 memoryTypeIndex, VkDeviceSize size);

			static void* mapSuballocation(VkDevice device, const MemorySuballocation& memorySuballocation, VkDeviceSize size, VkDeviceSize offset);

			static void unmapSuballocation(VkDevice device, uint32 memoryAllocationIndex);

			static void freeSuballocation(VkDevice device, uint32 memoryAllocationIndex);

		private:
			// Creates a new memory allocation.
			static void createAllocation(VkDevice device, uint32 memoryTypeIndex, VkDeviceSize size, std::vector<uint32>& currentMemoryAllocationIndices);

			// Creates and returns a suballocation from a memory allocation.
			// If isBuffer is false, the suballocation is done on image memory.
			static MemorySuballocation suballocate(VkDevice device, uint32 memoryTypeIndex, VkDeviceSize size, std::vector<uint32>& currentMemoryAllocationIndices);

		private:
			// Memory allocations have a default size of 16Mb.
			static constexpr VkDeviceSize DEFAULT_MEMORY_ALLOCATION_SIZE = 16000000;

		private:
			static const VulkanPhysicalDevice* physicalDevice;

			// Allocated buffer and image memory allocations.
			static std::vector<MemoryAllocation> memoryAllocations;

			// TODO: Merge with currentImageMemoryAllocationIndices.
			// Index of the currently active buffer memory allocation per memory type.
			static std::vector<uint32> currentBufferMemoryAllocationIndices;

			// Index of the currently active image memory allocation per memory type.
			static std::vector<uint32> currentImageMemoryAllocationIndices;
	};
}