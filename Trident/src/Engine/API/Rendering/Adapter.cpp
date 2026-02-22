// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Rendering/Buffer.hpp"
#include "Engine/API/Rendering/CommandPool.hpp"
#include "Engine/API/Rendering/Device.hpp"
#include "Engine/API/Rendering/PhysicalDevice.hpp"
#include "Engine/API/Rendering/PipelineLayout.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"

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

	ComputePipeline::ComputePipeline(const Device* device) :
		device(device)
	{}

	ComputePipeline::~ComputePipeline() {
		device->adapter->destroyComputePipeline(device, this);
	}

	DescriptorSetLayout::DescriptorSetLayout(const Device* device, const sequence<DescriptorSetLayoutBinding>& bindings) :
		device(device),
		bindings(bindings)
	{}

	DescriptorSetLayout::~DescriptorSetLayout() {
		device->adapter->destroyDescriptorSetLayout(device, this);
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

	GraphicsPipeline::GraphicsPipeline(const Device* device) :
		device(device)
	{}

	GraphicsPipeline::~GraphicsPipeline() {
		device->adapter->destroyGraphicsPipeline(device, this);
	}

	PipelineLayout::PipelineLayout(const Device* device) :
		device(device)
	{}

	PipelineLayout::~PipelineLayout() {
		device->adapter->destroyPipelineLayout(device, this);
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
		device->adapter->submit(this, commandBuffers);
	}

	void Queue::submit(view<ref<CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Fence> fence) const {
		device->adapter->submit(this, commandBuffers, waitSemaphore, signalSemaphore, fence);
	}

	PresentStatus Queue::present(view<ref<Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore) const {
		return device->adapter->present(this, swapchains, presentableImageIndices, waitSemaphore);
	}

	void Queue::waitIdle() const {
		device->adapter->waitIdle(this);
	}

	Semaphore::Semaphore(const Device* device) :
		device(device)
	{}

	Semaphore::~Semaphore() {
		device->adapter->destroySemaphore(device, this);
	}

	ShaderModule::ShaderModule(const Device* device) :
		device(device)
	{}

	ShaderModule::~ShaderModule() {
		device->adapter->destroyShaderModule(device, this);
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