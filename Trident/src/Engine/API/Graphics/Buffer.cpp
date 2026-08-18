// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/BufferView.hpp"
#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"

namespace Trident {
	Buffer::Buffer(const Device* device, const BufferDescriptor& descriptor) :
		device(device),
		size(descriptor.size),
		usage(descriptor.usage),
		memorySuballocation({
			.allocationIndex = max<uint32>(),
			.memoryProperties = descriptor.memoryProperties,
			.offset = 0,
			.size = 0,
		})
	{}

	Buffer::~Buffer() {
		device->adapter->destroyBufferImplementation(device, this);
	}

	uint64 Buffer::getSize() const {
		return size;
	}

	BufferUsage Buffer::getUsage() const {
		return usage;
	}

	MemorySuballocation Buffer::getMemorySuballocation() const {
		return memorySuballocation;
	}

	Access Buffer::getAccess() const {
		return access;
	}

	void* Buffer::map(uint64 offset, uint64 size) {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (size == 0) {
					VALIDATION_ERROR("The mapped range size must not be 0.");
				}

				if (memorySuballocation.allocationIndex == max<uint32>()) {
					VALIDATION_ERROR("The buffer is not allocated.");
				}

				if (!(memorySuballocation.memoryProperties & MemoryProperty::HOST_VISIBLE)) {
					VALIDATION_ERROR("The buffer isn't in a HOST_VISIBLE memory allocation.");
				}
			}
		#endif

		void* mappedRange = device->adapter->mapBufferImplementation(device, this, offset, size);

		MemoryAllocation& memoryAllocation = device->memoryAllocator->allocations[memorySuballocation.allocationIndex];
		memoryAllocation.mappedSuballocationCount += 1;

		return mappedRange;
	}

	void Buffer::unmap() {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (memorySuballocation.allocationIndex == max<uint32>()) {
					VALIDATION_ERROR("The buffer is not allocated.");
				}
				else {
					const MemoryAllocation& memoryAllocation = device->memoryAllocator->allocations[memorySuballocation.allocationIndex];

					if (memoryAllocation.mappedSuballocationCount == 0) {
						VALIDATION_ERROR("The buffer's parent memory allocation is not mapped.");
					}
				}
			}
		#endif

		device->adapter->unmapBufferImplementation(device, this);

		MemoryAllocation& memoryAllocation = device->memoryAllocator->allocations[memorySuballocation.allocationIndex];
		memoryAllocation.mappedSuballocationCount -= 1;
	}
}