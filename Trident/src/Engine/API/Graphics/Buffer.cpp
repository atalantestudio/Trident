// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/BufferView.hpp"
#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"

namespace Trident {
	BaseBuffer::BaseBuffer(const Device* device, const BufferDescriptor& descriptor) :
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

	BaseBuffer::~BaseBuffer() {
		device->adapter->destroyBufferImplementation(device, this);
	}

	uint64 BaseBuffer::getSize() const {
		return size;
	}

	/// Returns the buffer usage flags.
	BufferUsage BaseBuffer::getUsage() const {
		return usage;
	}

	MemorySuballocation BaseBuffer::getMemorySuballocation() const {
		return memorySuballocation;
	}

	/// Returns the current access flags for the buffer.
	Access BaseBuffer::getAccess() const {
		return access;
	}

	void* BaseBuffer::map(uint64 offset, uint64 size) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (size == 0) {
				ENGINE_ERROR("The mapped range size must not be 0.");

				errorCount += 1;
			}

			if (memorySuballocation.allocationIndex == max<uint32>()) {
				ENGINE_ERROR("The buffer hasn't been allocated yet.");

				errorCount += 1;
			}

			if (!(memorySuballocation.memoryProperties & MemoryProperty::HOST_VISIBLE)) {
				ENGINE_ERROR("The buffer isn't in a HOST_VISIBLE memory allocation.");

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		void* mappedRange = device->adapter->mapBufferImplementation(device, this, offset, size);

		MemoryAllocation& memoryAllocation = device->memoryAllocator->allocations[memorySuballocation.allocationIndex];
		memoryAllocation.mappedSuballocationCount += 1;

		return mappedRange;
	}

	void BaseBuffer::unmap() {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (memorySuballocation.allocationIndex == max<uint32>()) {
				ENGINE_ERROR("The buffer hasn't been allocated yet.");

				errorCount += 1;
			}

			{
				const MemoryAllocation& memoryAllocation = device->memoryAllocator->allocations[memorySuballocation.allocationIndex];

				if (memoryAllocation.mappedSuballocationCount == 0) {
					ENGINE_ERROR("The buffer's parent memory allocation is not mapped.");

					errorCount += 1;
				}
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->unmapBufferImplementation(device, this);

		MemoryAllocation& memoryAllocation = device->memoryAllocator->allocations[memorySuballocation.allocationIndex];
		memoryAllocation.mappedSuballocationCount -= 1;
	}
}