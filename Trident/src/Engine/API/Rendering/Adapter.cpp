// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Rendering/Buffer.hpp"
#include "Engine/API/Rendering/CommandPool.hpp"
#include "Engine/API/Rendering/Device.hpp"
#include "Engine/API/Rendering/ImageView.hpp"
#include "Engine/API/Rendering/PhysicalDevice.hpp"
#include "Engine/API/Rendering/PipelineLayout.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	BaseBuffer::BaseBuffer(const Device* device, const BufferDescriptor& descriptor) :
		device(device),
		size(descriptor.size),
		usage(descriptor.usage)
	{}

	BaseBuffer::~BaseBuffer() {
		device->adapter->destroyBuffer(device, this);
	}

	uint64 BaseBuffer::getSize() const {
		return size;
	}

	BufferUsage BaseBuffer::getUsage() const {
		return usage;
	}

	Access BaseBuffer::getAccess() const {
		return access;
	}

	void* BaseBuffer::map(uint64 offset, uint64 size) const {
		return device->adapter->mapBuffer(device, this, offset, size);
	}

	void BaseBuffer::unmap() const {
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

	void CommandEncoder::bindIndexBuffer(ref<BaseBuffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		device->adapter->bindIndexBuffer(this, indexBuffer, offset, indexTypeSize);
	}

	void CommandEncoder::bindVertexBuffer(uint32 bindingIndex, ref<BaseBuffer> vertexBuffer, uint64 offset, uint32 stride) {
		device->adapter->bindVertexBuffer(this, bindingIndex, vertexBuffer, offset, stride);
	}

	void CommandEncoder::copyBufferToBuffer(ref<BaseBuffer> sourceBuffer, ref<BaseBuffer> destinationBuffer, view<BufferCopyRegion> copyRegions) {
		device->adapter->copyBufferToBuffer(this, sourceBuffer, destinationBuffer, copyRegions);
	}

	void CommandEncoder::copyBufferToImage(ref<BaseBuffer> sourceBuffer, ref<Image> destinationImage, view<BufferImageCopyRegion> copyRegions) {
		device->adapter->copyBufferToImage(this, sourceBuffer, destinationImage, copyRegions);
	}

	void CommandEncoder::copyImageToBuffer(ref<Image> sourceImage, ref<BaseBuffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) {
		device->adapter->copyImageToBuffer(this, sourceImage, destinationBuffer, copyRegions);
	}

	void CommandEncoder::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		device->adapter->dispatch(this, groupCountX, groupCountY, groupCountZ);
	}

	void CommandEncoder::dispatchIndirect(ref<BaseBuffer> buffer, uint64 bufferOffset) {
		device->adapter->dispatchIndirect(this, buffer, bufferOffset);
	}

	void CommandEncoder::draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		device->adapter->draw(this, vertexCount, instanceCount, vertexOffset, instanceOffset);
	}

	void CommandEncoder::drawIndirect(ref<BaseBuffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		device->adapter->drawIndirect(this, buffer, bufferOffset, drawCount, stride);
	}

	void CommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		device->adapter->drawIndexed(this, indexCount, instanceCount, indexOffset, indexBias, instanceOffset);
	}

	void CommandEncoder::drawIndexedIndirect(ref<BaseBuffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
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

	Image::Image(const Device* device, const ImageDescriptor& descriptor) :
		device(device),
		type(descriptor.type),
		format(descriptor.format),
		extent(descriptor.extent),
		mipLevelCount(descriptor.mipLevelCount),
		layerCount(descriptor.layerCount),
		sampleCount(descriptor.sampleCount),
		layout(ImageLayout::UNDEFINED)
	{}

	Image::~Image() {
		device->adapter->destroyImage(device, this);
	}

	ImageType Image::getType() const {
		return type;
	}

	ImageFormat Image::getFormat() const {
		return format;
	}

	const Extent3D& Image::getExtent() const {
		return extent;
	}

	uint32 Image::getMipLevelCount() const {
		return mipLevelCount;
	}

	uint32 Image::getLayerCount() const {
		return layerCount;
	}

	uint8 Image::getSampleCount() const {
		return sampleCount;
	}

	ImageLayout Image::getLayout() const {
		return layout;
	}

	ref<ImageView> Image::createView(const ImageViewDescriptor& descriptor) {
		return device->adapter->createImageView(device, this, descriptor);
	}

	ImageView::ImageView(const Device* device, Image* image, const ImageViewDescriptor& descriptor) :
		device(device),
		image(image),
		mipLevelOffset(descriptor.mipLevelOffset),
		mipLevelCount(descriptor.mipLevelCount),
		layerOffset(descriptor.layerOffset),
		layerCount(descriptor.layerCount)
	{}

	ImageView::~ImageView() {
		device->adapter->destroyImageView(device, this);
	}

	Image* ImageView::getImage() {
		return image;
	}

	uint32 ImageView::getMipLevelOffset() const {
		return mipLevelOffset;
	}

	uint32 ImageView::getMipLevelCount() const {
		return mipLevelCount;
	}

	uint32 ImageView::getLayerOffset() const {
		return layerOffset;
	}

	uint32 ImageView::getLayerCount() const {
		return layerCount;
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

	RenderPass::RenderPass(const Device* device, const RenderPassDescriptor& descriptor) :
		device(device),
		attachments(descriptor.attachments),
		subpasses(descriptor.subpasses)
	{
		for (ref<Subpass> subpass : subpasses) {
			uint8 rasterizationSampleCount = 0;

			for (const AttachmentReference& colorAttachmentReference : subpass->getColorAttachmentReferences()) {
				if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
					continue;
				}

				const Attachment& attachment = attachments[colorAttachmentReference.index];

				if (attachment.sampleCount > 1) {
					attachmentReferenceCount += 1;
				}

				if (rasterizationSampleCount == 0) {
					rasterizationSampleCount = attachment.sampleCount;

					continue;
				}

				// TODO: There is no need for each attachment to have a sampleCount member.
				ENGINE_ASSERT(rasterizationSampleCount == attachment.sampleCount, "All color and depth/stencil attachments in a subpass must have the same sample count.");
			}

			if (subpass->getDepthStencilAttachmentReference().index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
				const AttachmentReference& depthStencilAttachmentReference = subpass->getDepthStencilAttachmentReference();

				// TODO: There is no need for each attachment to have a sampleCount member.
				ENGINE_ASSERT(rasterizationSampleCount == attachments[depthStencilAttachmentReference.index].sampleCount, "All color and depth/stencil attachments in a subpass must have the same sample count.");

				attachmentReferenceCount += 1;
			}

			DEBUG_ASSERT(rasterizationSampleCount > 0);

			subpass->setRasterizationSampleCount(rasterizationSampleCount);

			attachmentReferenceCount += subpass->getColorAttachmentReferenceCount() + subpass->getInputAttachmentReferenceCount();
		}
	}

	RenderPass::~RenderPass() {
		device->adapter->destroyRenderPass(device, this);
	}

	uint32 RenderPass::getAttachmentReferenceCount() const {
		return static_cast<uint32>(attachmentReferenceCount);
	}

	uint32 RenderPass::getAttachmentCount() const {
		return static_cast<uint32>(attachments.count());
	}

	const Attachment& RenderPass::getAttachment(uint32 attachmentIndex) const {
		if (attachmentIndex >= attachments.count()) {
			DEBUG_ASSERT(attachmentIndex < attachmentReferenceCount);

			// `attachmentIndex` refers to a resolve attachment.
			// Return the corresponding color attachment.
			return attachments[attachmentIndex - attachments.count()];
		}

		return attachments[attachmentIndex];
	}

	uint32 RenderPass::getSubpassCount() const {
		return static_cast<uint32>(subpasses.count());
	}

	ref<Subpass> RenderPass::getSubpass(uint32 subpassIndex) const {
		DEBUG_ASSERT(subpassIndex < subpasses.count());

		return subpasses[subpassIndex];
	}

	ref<Image> RenderPass::createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);
		DEBUG_ASSERT(attachmentIndex < attachments.count());

		const Attachment& attachment = attachments[attachmentIndex];

		DEBUG_ASSERT(attachment.sampleCount > 1 || (attachment.sampleCount == 1 && attachment.finalLayout != ImageLayout::PRESENT_SRC));

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = descriptor.type;
		imageDescriptor.format = attachment.format;
		imageDescriptor.extent = descriptor.extent;
		imageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		imageDescriptor.layerCount = descriptor.layerCount;
		imageDescriptor.sampleCount = attachment.sampleCount;
		imageDescriptor.usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT | descriptor.additionalUsage;
		imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		#if defined(BUILD_MODE_DEBUG)
			imageDescriptor.name = descriptor.name;
		#endif

		return Renderer::createImage(imageDescriptor);
	}

	ref<Image> RenderPass::createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 subpassIndex) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);
		DEBUG_ASSERT(subpassIndex < subpasses.count());

		const ref<Subpass> subpass = subpasses[subpassIndex];

		ENGINE_ASSERT(subpass->hasDepthStencilAttachmentReference(), "The subpass doesn't use a depth/stencil attachment.");

		const uint32 attachmentIndex = subpass->getDepthStencilAttachmentReference().index;

		DEBUG_ASSERT(attachmentIndex < attachments.count());

		const Attachment& attachment = attachments[attachmentIndex];

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = descriptor.type;
		imageDescriptor.format = static_cast<ImageFormat>(attachment.format);
		imageDescriptor.extent = descriptor.extent;
		imageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		imageDescriptor.layerCount = descriptor.layerCount;
		imageDescriptor.sampleCount = attachment.sampleCount;
		imageDescriptor.usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT | descriptor.additionalUsage;

		bool isInputAttachment = false;

		for (uint32 nextSubpassIndex = subpassIndex; nextSubpassIndex < subpasses.count(); nextSubpassIndex += 1) {
			const ref<Subpass> subpass = subpasses[nextSubpassIndex];

			for (const AttachmentReference& inputAttachmentReference : subpass->getInputAttachmentReferences()) {
				if (inputAttachmentReference.index == attachmentIndex) {
					isInputAttachment = true;

					break;
				}
			}

			if (isInputAttachment) {
				break;
			}
		}

		if (isInputAttachment) {
			imageDescriptor.usage |= ImageUsage::INPUT_ATTACHMENT;
		}

		imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		#if defined(BUILD_MODE_DEBUG)
			imageDescriptor.name = descriptor.name;
		#endif

		return Renderer::createImage(imageDescriptor);
	}

	ref<Image> RenderPass::createInputAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);
		DEBUG_ASSERT(attachmentIndex < attachments.count());

		const Attachment& attachment = attachments[attachmentIndex];

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = descriptor.type;
		imageDescriptor.format = attachment.format;
		imageDescriptor.extent = descriptor.extent;
		imageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		imageDescriptor.layerCount = descriptor.layerCount;
		imageDescriptor.sampleCount = attachment.sampleCount;
		imageDescriptor.usage = ImageUsage::INPUT_ATTACHMENT | descriptor.additionalUsage;

		if (isDepthFormat(attachment.format)) {
			imageDescriptor.usage |= ImageUsage::DEPTH_STENCIL_ATTACHMENT;
		}
		else {
			imageDescriptor.usage |= ImageUsage::COLOR_ATTACHMENT;
		}

		imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		#if defined(BUILD_MODE_DEBUG)
			imageDescriptor.name = descriptor.name;
		#endif

		return Renderer::createImage(imageDescriptor);
	}

	Sampler::Sampler(const Device* device) :
		device(device)
	{}

	Sampler::~Sampler() {
		device->adapter->destroySampler(device, this);
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
		instance(instance)
	{}

	Surface::~Surface() {
		instance->adapter->destroySurface(instance, this);
	}

	Swapchain::Swapchain(const Device* device, const SwapchainDescriptor& descriptor) :
		device(device),
		extent(descriptor.extent)
	{}

	Swapchain::~Swapchain() {
		device->adapter->destroySwapchain(device, this);
	}

	Extent2D Swapchain::getExtent() const {
		return extent;
	}

	uint32 Swapchain::getImageCount() const {
		return imageCount;
	}

	ref<ImageView> Swapchain::getImageView(uint32 index) const {
		ENGINE_ASSERT(index < imageViews.count(), "Image view index overflow.");

		return imageViews[index];
	}

	void Swapchain::update(const SwapchainDescriptor& descriptor) {
		device->adapter->updateSwapchain(device, this, descriptor);
	}
}