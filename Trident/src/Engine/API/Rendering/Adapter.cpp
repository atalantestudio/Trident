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

	void CommandEncoder::begin() const {
		device->adapter->begin(this);
	}

	void CommandEncoder::beginRenderPass(const BeginRenderPassDescriptor& descriptor) {
		device->adapter->beginRenderPass(this, descriptor);
	}

	void CommandEncoder::bindComputePipeline(ref<ComputePipeline> computePipeline) {
		device->adapter->bindComputePipeline(this, computePipeline);
	}

	void CommandEncoder::bindDescriptorSets(ref<PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<DescriptorSet>> descriptorSets) {
		device->adapter->bindDescriptorSets(this, pipelineLayout, descriptorSetIndexOffset, descriptorSets);
	}

	void CommandEncoder::bindGraphicsPipeline(ref<GraphicsPipeline> graphicsPipeline) {
		device->adapter->bindGraphicsPipeline(this, graphicsPipeline);
	}

	void CommandEncoder::bindIndexBuffer(ref<Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		device->adapter->bindIndexBuffer(this, indexBuffer, offset, indexTypeSize);
	}

	void CommandEncoder::bindVertexBuffer(uint32 bindingIndex, ref<Buffer> vertexBuffer, uint64 offset, uint32 stride) {
		device->adapter->bindVertexBuffer(this, bindingIndex, vertexBuffer, offset, stride);
	}

	void CommandEncoder::copyBufferToBuffer(ref<Buffer> sourceBuffer, ref<Buffer> destinationBuffer, view<BufferCopyRegion> copyRegions) {
		device->adapter->copyBufferToBuffer(this, sourceBuffer, destinationBuffer, copyRegions);
	}

	void CommandEncoder::copyBufferToImage(ref<Buffer> sourceBuffer, ref<Image> destinationImage, view<BufferImageCopyRegion> copyRegions) {
		device->adapter->copyBufferToImage(this, sourceBuffer, destinationImage, copyRegions);
	}

	void CommandEncoder::copyImageToBuffer(ref<Image> sourceImage, ref<Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) {
		device->adapter->copyImageToBuffer(this, sourceImage, destinationBuffer, copyRegions);
	}

	void CommandEncoder::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		device->adapter->dispatch(this, groupCountX, groupCountY, groupCountZ);
	}

	void CommandEncoder::dispatchIndirect(ref<Buffer> buffer, uint64 bufferOffset) {
		device->adapter->dispatchIndirect(this, buffer, bufferOffset);
	}

	void CommandEncoder::draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		device->adapter->draw(this, vertexCount, instanceCount, vertexOffset, instanceOffset);
	}

	void CommandEncoder::drawIndirect(ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		device->adapter->drawIndirect(this, buffer, bufferOffset, drawCount, stride);
	}

	void CommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		device->adapter->drawIndexed(this, indexCount, instanceCount, indexOffset, indexBias, instanceOffset);
	}

	void CommandEncoder::drawIndexedIndirect(ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		device->adapter->drawIndexedIndirect(this, buffer, bufferOffset, drawCount, stride);
	}

	ref<CommandBuffer> CommandEncoder::end() const {
		return device->adapter->end(this);
	}

	void CommandEncoder::endRenderPass() {
		device->adapter->endRenderPass(this);
	}

	void CommandEncoder::nextSubpass() {
		device->adapter->nextSubpass(this);
	}

	void CommandEncoder::insertBufferMemoryBarriers(view<BufferMemoryBarrier> barriers) {
		device->adapter->insertBufferMemoryBarriers(this, barriers);
	}

	void CommandEncoder::reset() {
		device->adapter->reset(this);
	}

	void CommandEncoder::setImageMemoryBarrier(ref<Image> image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) {
		device->adapter->setImageMemoryBarrier(this, image, newLayout, sourceStage, destinationStage);
	}

	void CommandEncoder::setLineWidth(float32 lineWidth) {
		device->adapter->setLineWidth(this, lineWidth);
	}

	void CommandEncoder::setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) {
		device->adapter->setScissor(this, xOffset, yOffset, extent);
	}

	void CommandEncoder::setViewport(vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) {
		device->adapter->setViewport(this, origin, size, minDepth, maxDepth);
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