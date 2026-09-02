// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"
#include "Engine/API/Graphics/PhysicalDevice.hpp"

namespace Trident {
	MemoryAllocator::MemoryAllocator(const Device* device) :
		device(device)
	{}

	uint32 MemoryAllocator::allocate(bool dynamic, uint32 memoryTypeIndex, uint64 size, uint64 alignment, bool shaderDeviceAddress) {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (size == 0) {
					VALIDATION_ERROR("The allocation size must not be 0.");
				}

				const uint32 maxMemoryAllocationCount = device->getPhysicalDevice()->getLimits().maxMemoryAllocationCount;

				if (allocations.size() >= maxMemoryAllocationCount) {
					VALIDATION_ERROR("There cannot be more than [] memory allocations on this device.", maxMemoryAllocationCount);
				}
			}
		#endif

		const MemoryAllocation allocation = device->adapter->allocateMemoryImplementation(this, dynamic, memoryTypeIndex, size, alignment, shaderDeviceAddress);

		uint32 allocationIndex;

		if (freeableAllocationIndices.size() > 0) {
			allocationIndex = freeableAllocationIndices.back();

			freeableAllocationIndices.pop_back();
		}
		else {
			allocationIndex = static_cast<uint32>(allocations.size());

			allocations.resize(allocations.size() + 1);
		}

		allocations[allocationIndex] = allocation;

		ENGINE_INFO("Created [] allocation ([]).", allocation.dynamic ? "dynamic" : "static", allocationIndex);
		ENGINE_DEBUG(
			"- Memory type = []\n"
			"- Size = []",
			allocation.memoryTypeIndex,
			allocation.size
		);

		#if defined(DEBUG_COUNT_ALLOCATED_MEMORY_SIZE)
			// TODO(graphics/memory-allocator): Also decrease when the allocation is freed.
			allocatedMemorySize += allocation.size;
		#endif

		return allocationIndex;
	}

	uint32 MemoryAllocator::findMemoryTypeIndex(MemoryProperty memoryProperties, bool transferDestination) const {
		return device->adapter->findMemoryTypeIndexImplementation(this, memoryProperties, transferDestination);
	}

	#if defined(DEBUG_COUNT_ALLOCATED_MEMORY_SIZE)
		uint64 MemoryAllocator::getAllocatedMemorySize() const {
			return allocatedMemorySize;
		}
	#endif

	MemoryRequirements MemoryAllocator::getMemoryRequirements(ref<Buffer> buffer) const {
		return device->adapter->getBufferMemoryRequirementsImplementation(this, buffer);
	}

	MemoryRequirements MemoryAllocator::getMemoryRequirements(ref<Image> image) const {
		return device->adapter->getImageMemoryRequirementsImplementation(this, image);
	}

	MemorySuballocation MemoryAllocator::suballocate(uint32 allocationIndex, uint64 size, uint64 alignment) {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (allocationIndex < allocations.size()) {
					const MemoryAllocation& allocation = allocations[allocationIndex];
					const uint64 offset = min(allocation.size, ceil(allocation.usedOffset, alignment));

					if (allocation.size - offset < size) {
						VALIDATION_ERROR("The suballocation requires [] bytes, but there are only [] bytes remaining in the allocation.", size, allocation.size - offset);
					}
				}
				else {
					VALIDATION_ERROR("The allocation index is overflowing.");
				}
			}
		#endif

		MemoryAllocation& allocation = allocations[allocationIndex];

		MemorySuballocation memorySuballocation{};
		memorySuballocation.allocationIndex = allocationIndex;
		memorySuballocation.offset = ceil(allocation.usedOffset, alignment);
		memorySuballocation.size = size;

		ENGINE_INFO("Created suballocation ([]) inside allocation [].", allocation.suballocationCount, memorySuballocation.allocationIndex);
		ENGINE_DEBUG(
			"- Offset = []\n"
			"- Size = []",
			memorySuballocation.offset,
			memorySuballocation.size
		);

		allocation.usedOffset = min(allocation.size, memorySuballocation.offset + memorySuballocation.size);
		allocation.suballocationCount += 1;

		return memorySuballocation;
	}

	void MemoryAllocator::suballocateStatic(uint32 allocationIndex, ref<Buffer> buffer) {
		buffer->memorySuballocation = device->adapter->suballocateBufferMemoryImplementation(this, allocationIndex, buffer);
	}

	void MemoryAllocator::suballocateStatic(uint32 allocationIndex, ref<Image> image) {
		image->memorySuballocation = device->adapter->suballocateImageMemoryImplementation(this, allocationIndex, image);
	}

	void MemoryAllocator::suballocateDynamic(ref<Buffer> buffer) {
		const MemoryRequirements memoryRequirements = getMemoryRequirements(buffer);
		const uint32 memoryTypeIndex = findMemoryTypeIndex(buffer->getMemorySuballocation().memoryProperties, buffer->getUsage() & BufferUsage::TRANSFER_DST);
		const bool shaderDeviceAddress = buffer->getUsage() & BufferUsage::SHADER_DEVICE_ADDRESS;

		uint32 allocationIndex = findAllocationIndex(memoryTypeIndex, memoryRequirements, shaderDeviceAddress);
		bool needAllocation = false;

		if (allocationIndex >= allocations.size()) {
			needAllocation = true;
		}
		else {
			for (const uint32 freeableAllocationIndex : freeableAllocationIndices) {
				if (allocationIndex == freeableAllocationIndex) {
					needAllocation = true;

					break;
				}
			}
		}

		if (needAllocation) {
			allocationIndex = allocate(true, memoryTypeIndex, max(MIN_DYNAMIC_MEMORY_ALLOCATION_SIZE, memoryRequirements.size), memoryRequirements.alignment, shaderDeviceAddress);
		}

		suballocateStatic(allocationIndex, buffer);
	}

	void MemoryAllocator::suballocateDynamic(ref<Image> image) {
		const MemoryRequirements memoryRequirements = getMemoryRequirements(image);
		const uint32 memoryTypeIndex = findMemoryTypeIndex(image->getMemorySuballocation().memoryProperties, image->getUsage() & ImageUsage::TRANSFER_DST);

		// TODO(graphics/memory-allocator/image-shader-device-address): Add support for SHADER_DEVICE_ADDRESS for images?
		uint32 allocationIndex = findAllocationIndex(memoryTypeIndex, memoryRequirements, false);

		bool needAllocation = false;

		if (allocationIndex >= allocations.size()) {
			needAllocation = true;
		}
		else {
			for (const uint32 freeableAllocationIndex : freeableAllocationIndices) {
				if (allocationIndex == freeableAllocationIndex) {
					needAllocation = true;

					break;
				}
			}
		}

		if (needAllocation) {
			// TODO(graphics/memory-allocator/image-shader-device-address): Add support for SHADER_DEVICE_ADDRESS for images?
			allocationIndex = allocate(true, memoryTypeIndex, max(MIN_DYNAMIC_MEMORY_ALLOCATION_SIZE, memoryRequirements.size), memoryRequirements.alignment, false);
		}

		suballocateStatic(allocationIndex, image);
	}

	uint32 MemoryAllocator::findAllocationIndex(uint32 memoryTypeIndex, const MemoryRequirements& memoryRequirements, bool shaderDeviceAddress) const {
		uint32 allocationIndex = 0;

		while (allocationIndex < allocations.size()) {
			const MemoryAllocation& allocation = allocations[allocationIndex];

			if (!allocation.dynamic) {
				allocationIndex += 1;

				continue;
			}

			if (allocation.memoryTypeIndex != memoryTypeIndex) {
				allocationIndex += 1;

				continue;
			}

			if (allocation.shaderDeviceAddress != shaderDeviceAddress) {
				allocationIndex += 1;

				continue;
			}

			const uint64 offset = min(allocation.size, ceil(allocation.usedOffset, memoryRequirements.alignment));

			if (allocation.size - offset < memoryRequirements.size) {
				allocationIndex += 1;

				continue;
			}

			break;
		}

		return allocationIndex;
	}
}