// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/GL/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/VK/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/Device.hpp"
#include "Engine/API/Rendering/Instance.hpp"

// TODO: Restore descriptor validation.
namespace Trident {
	ConsoleLogger GraphicsAdapter::logger(std::cout, LogLevel::ALL, "");

	GraphicsAdapter::GraphicsAdapter(const ApplicationProperties& properties) :
		api(properties.GraphicsAPI)
	{
		static constexpr BindAdapterImplementation BIND_ADAPTER_IMPLEMENTATIONS[static_cast<index>(GraphicsAPI::count)] = {
			DX12::bindAdapter,
			GL::bindAdapter,
			VK::bindAdapter,
		};

		DEBUG_ASSERT(properties.GraphicsAPI < GraphicsAPI::count);

		BIND_ADAPTER_IMPLEMENTATIONS[static_cast<index>(properties.GraphicsAPI)](this);
	}

	GraphicsAPI GraphicsAdapter::getApi() const {
		return api;
	}

	AcquireNextImageStatus GraphicsAdapter::acquireNextImage(const Device* device, const Swapchain* swapchain, const Semaphore* semaphore, uint32& imageIndex) const {
		return acquireNextImageImplementation(device, swapchain, semaphore, imageIndex);
	}

	sequence<ref<DescriptorSet>> GraphicsAdapter::allocateDescriptorSets(Device* device, view<ref<DescriptorSetLayout>> descriptorSetLayouts) const {
		return allocateDescriptorSetsImplementation(device, descriptorSetLayouts);
	}

	void GraphicsAdapter::begin(const CommandEncoder* commandEncoder) const {
		beginImplementation(commandEncoder);
	}

	void GraphicsAdapter::beginRenderPass(CommandEncoder* commandEncoder, const BeginRenderPassDescriptor& descriptor) const {
		beginRenderPassImplementation(commandEncoder, descriptor);
	}

	void GraphicsAdapter::bindComputePipeline(CommandEncoder* commandEncoder, ref<ComputePipeline> computePipeline) const {
		bindComputePipelineImplementation(commandEncoder, computePipeline);
	}

	void GraphicsAdapter::bindDescriptorSets(CommandEncoder* commandEncoder, ref<PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<DescriptorSet>> descriptorSets) const {
		bindDescriptorSetsImplementation(commandEncoder, pipelineLayout, descriptorSetIndexOffset, descriptorSets);
	}

	void GraphicsAdapter::bindGraphicsPipeline(CommandEncoder* commandEncoder, ref<GraphicsPipeline> graphicsPipeline) const {
		bindGraphicsPipelineImplementation(commandEncoder, graphicsPipeline);
	}

	void GraphicsAdapter::bindIndexBuffer(CommandEncoder* commandEncoder, ref<BaseBuffer> indexBuffer, uint64 offset, uint8 indexTypeSize) const {
		bindIndexBufferImplementation(commandEncoder, indexBuffer, offset, indexTypeSize);
	}

	void GraphicsAdapter::bindVertexBuffer(CommandEncoder* commandEncoder, uint32 bindingIndex, ref<BaseBuffer> vertexBuffer, uint64 offset, uint32 stride) const {
		bindVertexBufferImplementation(commandEncoder, bindingIndex, vertexBuffer, offset, stride);
	}

	void GraphicsAdapter::copyBufferToBuffer(CommandEncoder* commandEncoder, ref<BaseBuffer> sourceBuffer, ref<BaseBuffer> destinationBuffer, view<BufferCopyRegion> copyRegions) const {
		copyBufferToBufferImplementation(commandEncoder, sourceBuffer, destinationBuffer, copyRegions);
	}

	void GraphicsAdapter::copyBufferToImage(CommandEncoder* commandEncoder, ref<BaseBuffer> sourceBuffer, ref<Image> destinationImage, view<BufferImageCopyRegion> copyRegions) const {
		copyBufferToImageImplementation(commandEncoder, sourceBuffer, destinationImage, copyRegions);
	}

	void GraphicsAdapter::copyImageToBuffer(CommandEncoder* commandEncoder, ref<Image> sourceImage, ref<BaseBuffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) const {
		copyImageToBufferImplementation(commandEncoder, sourceImage, destinationBuffer, copyRegions);
	}

	ref<CommandEncoder> GraphicsAdapter::createCommandEncoder(const Device* device, const CommandPool* commandPool, const CommandBufferDescriptor& descriptor) const {
		return createCommandEncoderImplementation(device, commandPool, descriptor);
	}

	ref<CommandPool> GraphicsAdapter::createCommandPool(const Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) const {
		return createCommandPoolImplementation(device, queueFamilyIndex, queueFamilyCapability);
	}

	ref<ComputePipeline> GraphicsAdapter::createComputePipeline(const Device* device, const ComputePipelineDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createComputePipelineImplementation(device, descriptor);
	}

	ref<DescriptorSetLayout> GraphicsAdapter::createDescriptorSetLayout(const Device* device, const sequence<DescriptorSetLayoutBinding>& bindings) const {
		return createDescriptorSetLayoutImplementation(device, bindings);
	}

	ref<Device> GraphicsAdapter::createDevice(const PhysicalDevice* physicalDevice) const {
		ref<Device> device = createDeviceImplementation(this, physicalDevice);
		device->memoryAllocator = createMemoryAllocatorImplementation(device.get());

		return device;
	}

	ref<Fence> GraphicsAdapter::createFence(const Device* device, const FenceDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createFenceImplementation(device, descriptor);
	}

	ref<Framebuffer> GraphicsAdapter::createFramebuffer(const Device* device, const FramebufferDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createFramebufferImplementation(device, descriptor);
	}

	ref<Image> GraphicsAdapter::createImage(const Device* device, const ImageDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createImageImplementation(device, descriptor);
	}

	ref<ImageView> GraphicsAdapter::createImageView(const Device* device, Image* image, const ImageViewDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor, image);

		return createImageViewImplementation(device, image, descriptor);
	}

	ref<Instance> GraphicsAdapter::createInstance(const InstanceDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createInstanceImplementation(this, descriptor);
	}

	ref<PipelineLayout> GraphicsAdapter::createPipelineLayout(const Device* device, const PipelineLayoutDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createPipelineLayoutImplementation(device, descriptor);
	}

	ref<RenderPass> GraphicsAdapter::createRenderPass(const Device* device, ref<Surface> surface, const RenderPassDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createRenderPassImplementation(device, surface, descriptor);
	}

	ref<GraphicsPipeline> GraphicsAdapter::createGraphicsPipeline(const Device* device, const GraphicsPipelineDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createGraphicsPipelineImplementation(device, descriptor);
	}

	ref<Sampler> GraphicsAdapter::createSampler(const Device* device, const SamplerDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createSamplerImplementation(device, descriptor);
	}

	ref<Semaphore> GraphicsAdapter::createSemaphore(const Device* device) const {
		return createSemaphoreImplementation(device);
	}

	ref<ShaderModule> GraphicsAdapter::createShaderModule(const Device* device, const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createShaderModuleFromAssetImplementation(device, descriptor, asset);
	}

	ref<ShaderModule> GraphicsAdapter::createShaderModule(const Device* device, const ShaderModuleDescriptor& descriptor, view<char8> code) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createShaderModuleFromCodeImplementation(device, descriptor, code);
	}

	ref<Surface> GraphicsAdapter::createSurface(const Instance* instance, ref<PhysicalDevice> physicalDevice, const Window* window) const {
		return createSurfaceImplementation(instance, physicalDevice, window);
	}

	ref<Swapchain> GraphicsAdapter::createSwapchain(const Device* device, const SwapchainDescriptor& descriptor) const {
		return createSwapchainImplementation(device, descriptor);
	}

	void GraphicsAdapter::destroyBuffer(const Device* device, const BaseBuffer* buffer) const {
		destroyBufferImplementation(device, buffer);
	}

	void GraphicsAdapter::destroyCommandEncoder(const Device* device, const CommandPool* commandPool, const CommandEncoder* commandEncoder) const {
		destroyCommandEncoderImplementation(device, commandPool, commandEncoder);
	}

	void GraphicsAdapter::destroyCommandPool(const Device* device, const CommandPool* commandPool) const {
		destroyCommandPoolImplementation(device, commandPool);
	}

	void GraphicsAdapter::destroyComputePipeline(const Device* device, const ComputePipeline* computePipeline) const {
		destroyComputePipelineImplementation(device, computePipeline);
	}

	void GraphicsAdapter::destroyDescriptorSetLayout(const Device* device, const DescriptorSetLayout* descriptorSetLayout) const {
		destroyDescriptorSetLayoutImplementation(device, descriptorSetLayout);
	}

	void GraphicsAdapter::destroyDevice(const Device* device) const {
		destroyDeviceImplementation(device);
	}

	void GraphicsAdapter::destroyFence(const Device* device, const Fence* fence) const {
		destroyFenceImplementation(device, fence);
	}

	void GraphicsAdapter::destroyFramebuffer(const Device* device, const Framebuffer* framebuffer) const {
		destroyFramebufferImplementation(device, framebuffer);
	}

	void GraphicsAdapter::destroyGraphicsPipeline(const Device* device, const GraphicsPipeline* graphicsPipeline) const {
		destroyGraphicsPipelineImplementation(device, graphicsPipeline);
	}

	void GraphicsAdapter::destroyImage(const Device* device, const Image* image) const {
		destroyImageImplementation(device, image);
	}

	void GraphicsAdapter::destroyImageView(const Device* device, const ImageView* imageView) const {
		destroyImageViewImplementation(device, imageView);
	}

	void GraphicsAdapter::destroyInstance(const Instance* instance) const {
		destroyInstanceImplementation(instance);
	}

	void GraphicsAdapter::destroyPipelineLayout(const Device* device, const PipelineLayout* pipelineLayout) const {
		destroyPipelineLayoutImplementation(device, pipelineLayout);
	}

	void GraphicsAdapter::destroyQueue(const Queue* queue) const {
		destroyQueueImplementation(queue);
	}

	void GraphicsAdapter::destroyRenderPass(const Device* device, const RenderPass* renderPass) const {
		destroyRenderPassImplementation(device, renderPass);
	}

	void GraphicsAdapter::destroySampler(const Device* device, const Sampler* sampler) const {
		destroySamplerImplementation(device, sampler);
	}

	void GraphicsAdapter::destroySemaphore(const Device* device, const Semaphore* semaphore) const {
		destroySemaphoreImplementation(device, semaphore);
	}

	void GraphicsAdapter::destroyShaderModule(const Device* device, const ShaderModule* shaderModule) const {
		destroyShaderModuleImplementation(device, shaderModule);
	}

	void GraphicsAdapter::destroySurface(const Instance* instance, const Surface* surface) const {
		destroySurfaceImplementation(instance, surface);
	}

	void GraphicsAdapter::destroySwapchain(const Device* device, const Swapchain* swapchain) const {
		destroySwapchainImplementation(device, swapchain);
	}

	void GraphicsAdapter::dispatch(CommandEncoder* commandEncoder, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) const {
		dispatchImplementation(commandEncoder, groupCountX, groupCountY, groupCountZ);
	}

	void GraphicsAdapter::dispatchIndirect(CommandEncoder* commandEncoder, ref<BaseBuffer> buffer, uint64 bufferOffset) const {
		dispatchIndirectImplementation(commandEncoder, buffer, bufferOffset);
	}

	void GraphicsAdapter::draw(CommandEncoder* commandEncoder, uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) const {
		drawImplementation(commandEncoder, vertexCount, instanceCount, vertexOffset, instanceOffset);
	}

	void GraphicsAdapter::drawIndirect(CommandEncoder* commandEncoder, ref<BaseBuffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) const {
		drawIndirectImplementation(commandEncoder, buffer, bufferOffset, drawCount, stride);
	}

	void GraphicsAdapter::drawIndexed(CommandEncoder* commandEncoder, uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) const {
		drawIndexedImplementation(commandEncoder, indexCount, instanceCount, indexOffset, indexBias, instanceOffset);
	}

	void GraphicsAdapter::drawIndexedIndirect(CommandEncoder* commandEncoder, ref<BaseBuffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) const {
		drawIndexedIndirectImplementation(commandEncoder, buffer, bufferOffset, drawCount, stride);
	}

	ref<CommandBuffer> GraphicsAdapter::end(const CommandEncoder* commandEncoder) const {
		return endImplementation(commandEncoder);
	}

	void GraphicsAdapter::endRenderPass(CommandEncoder* commandEncoder) const {
		endRenderPassImplementation(commandEncoder);
	}

	ref<Queue> GraphicsAdapter::getQueue(const Device* device, QueueFamilyCapability queueFamilyCapability) const {
		return getQueueImplementation(device, queueFamilyCapability);
	}

	void GraphicsAdapter::insertMemoryBarriers(CommandEncoder* commandEncoder, view<BufferMemoryBarrier> bufferMemoryBarriers, view<ImageMemoryBarrier> imageMemoryBarriers) const {
		insertMemoryBarriersImplementation(commandEncoder, bufferMemoryBarriers, imageMemoryBarriers);
	}

	void* GraphicsAdapter::mapBuffer(const Device* device, const BaseBuffer* buffer, uint64 offset, uint64 size) const {
		return mapBufferImplementation(device, buffer, offset, size);
	}

	void GraphicsAdapter::nextSubpass(CommandEncoder* commandEncoder) const {
		nextSubpassImplementation(commandEncoder);
	}

	PresentStatus GraphicsAdapter::present(const Queue* queue, view<ref<Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore) const {
		return presentImplementation(queue, swapchains, presentableImageIndices, waitSemaphore);
	}

	void GraphicsAdapter::reset(CommandEncoder* commandEncoder) const {
		resetImplementation(commandEncoder);
	}

	void GraphicsAdapter::resetFence(const Device* device, const Fence* fence) const {
		resetFenceImplementation(device, fence);
	}

	ref<PhysicalDevice> GraphicsAdapter::selectPhysicalDevice(const Instance* instance, bool preferIntegratedGpu) const {
		return selectPhysicalDeviceImplementation(instance, preferIntegratedGpu);
	}

	void GraphicsAdapter::setLineWidth(CommandEncoder* commandEncoder, float32 lineWidth) const {
		setLineWidthImplementation(commandEncoder, lineWidth);
	}

	void GraphicsAdapter::setScissor(CommandEncoder* commandEncoder, sint32 xOffset, sint32 yOffset, Extent2D extent) const {
		setScissorImplementation(commandEncoder, xOffset, yOffset, extent);
	}

	void GraphicsAdapter::setViewport(CommandEncoder* commandEncoder, vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) const {
		setViewportImplementation(commandEncoder, origin, size, minDepth, maxDepth);
	}

	void GraphicsAdapter::submit(const Queue* queue, view<ref<CommandBuffer>> commandBuffers) const {
		submitImplementation(queue, commandBuffers);
	}

	void GraphicsAdapter::submit(const Queue* queue, view<ref<CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Fence> fence) const {
		submit2Implementation(queue, commandBuffers, waitSemaphore, signalSemaphore, fence);
	}

	void GraphicsAdapter::unmapBuffer(const Device* device, const BaseBuffer* buffer) const {
		unmapBufferImplementation(device, buffer);
	}

	void GraphicsAdapter::updateDescriptorSets(const Device* device, view<WriteDescriptorSet> writeDescriptorSets) const {
		updateDescriptorSetsImplementation(device, writeDescriptorSets);
	}

	void GraphicsAdapter::updateSwapchain(const Device* device, Swapchain* swapchain, const SwapchainDescriptor& descriptor) const {
		updateSwapchainImplementation(device, swapchain, descriptor);
	}

	void GraphicsAdapter::waitForFence(const Device* device, Fence* fence) const {
		waitForFenceImplementation(device, fence);
	}

	void GraphicsAdapter::waitIdle(const Device* device) const {
		waitIdleDeviceImplementation(device);
	}

	void GraphicsAdapter::waitIdle(const Queue* queue) const {
		waitIdleQueueImplementation(queue);
	}
}