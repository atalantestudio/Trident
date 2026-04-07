// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/Device.hpp"

namespace Trident {
	BaseBuffer::BaseBuffer(const Device* device, const BufferDescriptor& descriptor, uint32 stride) :
		device(device),
		size(descriptor.size),
		usage(descriptor.usage),
		stride(stride)
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

	/// Returns the current access flags for the buffer.
	Access BaseBuffer::getAccess() const {
		return access;
	}

	void* BaseBuffer::map(uint64 offset, uint64 size) const {
		return device->adapter->mapBufferImplementation(device, this, offset, size);
	}

	void BaseBuffer::unmap() const {
		device->adapter->unmapBufferImplementation(device, this);
	}
}