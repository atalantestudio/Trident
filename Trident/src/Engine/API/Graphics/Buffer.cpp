// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"

namespace Trident {
	BaseBuffer::BaseBuffer(const Device* device, const BufferDescriptor& descriptor, uint32 stride) :
		device(device),
		size(descriptor.size),
		stride(stride),
		usage(descriptor.usage)
	{}

	BaseBuffer::~BaseBuffer() {
		device->adapter->destroyBufferImplementation(device, this);
	}

	uint64 BaseBuffer::getSize() const {
		return size;
	}

	uint32 BaseBuffer::getStride() const {
		return stride;
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

	void* BaseBuffer::map(uint64 offset, uint64 size) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 warningCount = 0;
			uint32 errorCount = 0;

			if (offset >= size) {
				ENGINE_ERROR("The mapped range offset must be less than its size.");

				errorCount += 1;
			}

			if (size == 0) {
				ENGINE_ERROR("The mapped range size must not be 0.");

				errorCount += 1;
			}

			const MemoryAllocation& memoryAllocation = device->memoryAllocator->allocations[memorySuballocation.allocationIndex];

			if (!(memoryAllocation.memoryProperties & MemoryProperty::HOST_VISIBLE)) {
				ENGINE_ERROR("The buffer isn't in a HOST_VISIBLE memory allocation.");

				errorCount += 1;
			}

			if (memoryAllocation.mappedSuballocationCount > 0) {
				ENGINE_ERROR("The buffer's parent memory allocation is already mapped.");

				errorCount += 1;
			}

			ENGINE_ASSERT(warningCount == 0 && errorCount == 0, "Invalid argument(s) ([] warnings and [] errors).", warningCount, errorCount);
		#endif

		return device->adapter->mapBufferImplementation(device, this, offset, size);
	}

	void BaseBuffer::unmap() const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 warningCount = 0;
			uint32 errorCount = 0;

			const MemoryAllocation& memoryAllocation = device->memoryAllocator->allocations[memorySuballocation.allocationIndex];

			if (memoryAllocation.mappedSuballocationCount == 0) {
				ENGINE_ERROR("The buffer's parent memory allocation is not mapped.");

				errorCount += 1;
			}

			ENGINE_ASSERT(warningCount == 0 && errorCount == 0, "Invalid argument(s) ([] warnings and [] errors).", warningCount, errorCount);
		#endif

		device->adapter->unmapBufferImplementation(device, this);
	}
}