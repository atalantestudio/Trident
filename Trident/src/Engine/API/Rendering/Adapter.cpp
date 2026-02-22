// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Rendering/Buffer.hpp"
#include "Engine/API/Rendering/CommandPool.hpp"
#include "Engine/API/Rendering/Device.hpp"
#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace Trident {
	Buffer::Buffer(const Device* device, const BufferDescriptor& descriptor) :
		device(device),
		size(descriptor.size),
		usage(descriptor.usage)
	{}

	Buffer::~Buffer() {
		device->adapter->destroyBuffer(device, this);
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
		return device->adapter->mapBuffer(device, this, offset, size);
	}

	void Buffer::unmap() const {
		device->adapter->unmapBuffer(device, this);
	}

	CommandEncoder::CommandEncoder(const Device* device, const CommandPool* commandPool) :
		device(device),
		commandPool(commandPool)
	{}

	CommandEncoder::~CommandEncoder() {
		device->adapter->destroyCommandEncoder(device, commandPool, this);
	}

	CommandPool::CommandPool(const Device* device) :
		device(device)
	{}

	CommandPool::~CommandPool() {
		device->adapter->destroyCommandPool(device, this);
	}

	Fence::Fence(const Device* device) :
		device(device)
	{}

	Fence::~Fence() {
		device->adapter->destroyFence(device, this);
	}

	Framebuffer::~Framebuffer() {
		device->adapter->destroyFramebuffer(device, this);
	}

	Queue::Queue(const Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) :
		device(device),
		commandPool(device->adapter->createCommandPool(device, queueFamilyIndex, queueFamilyCapability))
	{}

	Queue::~Queue() {
		device->adapter->destroyQueue(this);
	}

	ref<CommandEncoder> Queue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return device->adapter->createCommandEncoder(device, commandPool.get(), descriptor);
	}

	void Queue::submit(view<ref<CommandBuffer>> commandBuffers) const {
		// TODO
		BREAKPOINT();
	}

	void Queue::submit(view<ref<CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Fence> fence) const {
		// TODO
		BREAKPOINT();
	}

	PresentStatus Queue::present(view<ref<Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore) const {
		// TODO
		BREAKPOINT();

		return PresentStatus::SUCCESS;
	}

	void Queue::waitIdle() const {
		// TODO
		BREAKPOINT();
	}

	Surface::Surface(const Instance* instance) :
		instance(instance),
		format(instance->adapter->getSurfaceFormat(this))
	{}

	Surface::~Surface() {
		instance->adapter->destroySurface(instance, this);
	}

	Swapchain::~Swapchain() {
		device->adapter->destroySwapchain(device, this);
	}
}