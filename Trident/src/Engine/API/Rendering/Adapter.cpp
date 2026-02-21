// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Rendering/Buffer.hpp"
#include "Engine/API/Rendering/Device.hpp"
#include "Engine/API/Rendering/PhysicalDevice.hpp"
// #include "Engine/API/Rendering/Queue.hpp"

namespace Trident {
	Buffer::~Buffer() {
		device->destroyBuffer(this);
	}

	/// Returns the size in bytes of the buffer.
	uint64 Buffer::getSize() const {
		return size;
	}

	/// Returns the current buffer usage flags.
	BufferUsage Buffer::getUsage() const {
		return usage;
	}

	/// Returns the current buffer access flags.
	Access Buffer::getAccess() const {
		return access;
	}

	void* Buffer::map(uint64 offset, uint64 size) const {
		return device->mapBuffer(this, offset, size);
	}

	/// Unmaps the buffer.
	void Buffer::unmap() const {
		device->unmapBuffer(this);
	}

	Buffer::Buffer(const Device* device, const BufferDescriptor& descriptor) :
		device(device),
		size(descriptor.size),
		usage(descriptor.usage)
	{
		// TODO: ?
		// adapter->createBuffer(this, descriptor);
	}

	ref<PhysicalDevice> Device::getPhysicalDevice() const {
		return createRef<PhysicalDevice>(*physicalDevice);
	}

	// TODO
	/* ref<CommandEncoder> Queue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return device->createCommandEncoder();
	} */
}