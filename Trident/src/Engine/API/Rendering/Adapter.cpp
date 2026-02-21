// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Rendering/Buffer.hpp"
#include "Engine/API/Rendering/Device.hpp"
#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace Trident {
	Buffer::~Buffer() {
		device->destroyBuffer(this);
	}

	uint64 Buffer::getSize() const {
		return size;
	}

	BufferUsage Buffer::getUsage() const {
		return usage;
	}

	Access Buffer::getAccess() const {
		return access;
	}

	void* Buffer::map(uint64 offset, uint64 size) const {
		return device->mapBuffer(this, offset, size);
	}

	void Buffer::unmap() const {
		device->unmapBuffer(this);
	}

	Buffer::Buffer(const Device* device, const BufferDescriptor& descriptor) :
		device(device),
		size(descriptor.size),
		usage(descriptor.usage)
	{}

	Surface::Surface(const Instance* instance) :
		instance(createRef<Instance>(*instance)),
		format(instance->adapter->getSurfaceFormat(this))
	{}

	Surface::~Surface() {
		instance->adapter->destroySurface(instance.get(), this);
	}

	Swapchain::~Swapchain() {
		device->adapter->destroySwapchain(device, this);
	}
}