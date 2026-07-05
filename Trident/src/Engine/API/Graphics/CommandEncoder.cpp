// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/CommandEncoder.hpp"

namespace Trident {
	CommandEncoder::CommandEncoder(const Device* device, const CommandPool* commandPool) :
		device(device),
		commandPool(commandPool)
	{}

	CommandEncoder::~CommandEncoder() {
		device->adapter->destroyCommandEncoderImplementation(device, commandPool, this);
	}

	void CommandEncoder::begin() const {
		device->adapter->beginImplementation(this);
	}

	void CommandEncoder::beginRenderPass(const BeginRenderPassDescriptor& descriptor) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (descriptor.framebuffer == nullptr) {
				ENGINE_ERROR("descriptor.framebuffer must be a valid Framebuffer object.");

				errorCount += 1;
			}
			else {
				const ref<RenderPass> renderPass = descriptor.framebuffer->getRenderPass();

				if (renderPass == nullptr) {
					ENGINE_ERROR("descriptor.framebuffer.renderPass must be a valid RenderPass object.");

					errorCount += 1;
				}
				else {
					if (descriptor.framebuffer->getImageViews().count() != renderPass->getTotalAttachmentCount()) {
						ENGINE_ERROR("descriptor.framebuffer must have one ImageView object for each attachment in the render pass. This includes the implicit resolve attachment for the swapchain image.");

						errorCount += 1;
					}

					if (descriptor.clearValues.count() < renderPass->getAttachmentCount()) {
						ENGINE_ERROR("descriptor.clearValues.count must be greater than or equal to the number of attachments in descriptor.framebuffer.renderPass.");

						errorCount += 1;
					}

					// TODO(graphics/validation-layer): Restore.
					/* if (descriptor.clearValues.count() > renderPass->getAttachmentReferenceCount()) {
						ENGINE_ERROR("descriptor.clearValues.count must be smaller than the number of attachment references in descriptor.framebuffer.renderPass.");

						errorCount += 1;
					} */
				}
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->beginRenderPassImplementation(this, descriptor);

		currentFramebuffer = descriptor.framebuffer;
	}

	void CommandEncoder::bindComputePipeline(ref<ComputePipeline> computePipeline) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (computePipeline == nullptr) {
				ENGINE_ERROR("computePipeline must be a valid ComputePipeline object.");

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->bindComputePipelineImplementation(this, computePipeline);
	}

	void CommandEncoder::bindDescriptorSets(ref<PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<DescriptorSet>> descriptorSets) {
		// TODO(graphics/validation/null-object-guard)

		device->adapter->bindDescriptorSetsImplementation(this, pipelineLayout, descriptorSetIndexOffset, descriptorSets);
	}

	void CommandEncoder::bindGraphicsPipeline(ref<GraphicsPipeline> graphicsPipeline) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (graphicsPipeline == nullptr) {
				ENGINE_ERROR("graphicsPipeline must be a valid GraphicsPipeline object.");

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->bindGraphicsPipelineImplementation(this, graphicsPipeline);
	}

	void CommandEncoder::bindIndexBuffer(ref<Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (indexBuffer == nullptr) {
				ENGINE_ERROR("indexBuffer must be a valid Buffer object.");

				errorCount += 1;
			}
			else {
				if (!(indexBuffer->getUsage() & BufferUsage::INDEX)) {
					ENGINE_ERROR("indexBuffer must have the INDEX usage flag.");

					errorCount += 1;
				}

				if (offset >= indexBuffer->getSize()) {
					ENGINE_ERROR("bufferOffset must be less than the size of indexBuffer.");

					errorCount += 1;
				}
			}

			if (indexTypeSize == 0) {
				ENGINE_ERROR("The index size must not be 0.");

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->bindIndexBufferImplementation(this, indexBuffer, offset, indexTypeSize);
	}

	void CommandEncoder::bindVertexBuffer(uint32 bindingIndex, ref<Buffer> vertexBuffer, uint64 offset, uint32 stride) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			// TODO(graphics/command-encoder/bind-vertex-buffer): Validate the binding index.

			if (vertexBuffer == nullptr) {
				ENGINE_ERROR("vertexBuffer must be a valid Buffer object.");

				errorCount += 1;
			}
			else {
				if (!(vertexBuffer->getUsage() & BufferUsage::VERTEX)) {
					ENGINE_ERROR("vertexBuffer must have the VERTEX usage flag.");

					errorCount += 1;
				}

				if (offset >= vertexBuffer->getSize()) {
					ENGINE_ERROR("bufferOffset must be less than the size of vertexBuffer.");

					errorCount += 1;
				}
			}

			if (stride == 0) {
				ENGINE_ERROR("stride must not be 0.");

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->bindVertexBufferImplementation(this, bindingIndex, vertexBuffer, offset, stride);
	}

	void CommandEncoder::copyBufferToBuffer(ref<Buffer> sourceBuffer, ref<Buffer> destinationBuffer, view<BufferCopyRegion> copyRegions) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (sourceBuffer == nullptr) {
				ENGINE_ERROR("sourceBuffer must be a valid Image object.");

				errorCount += 1;
			}
			else {
				if (!(sourceBuffer->getUsage() & BufferUsage::TRANSFER_SRC)) {
					ENGINE_ERROR("sourceBuffer must have the TRANSFER_SRC usage flag.");

					errorCount += 1;
				}
			}

			if (destinationBuffer == nullptr) {
				ENGINE_ERROR("destinationBuffer must be a valid Buffer object.");

				errorCount += 1;
			}
			else {
				if (!(destinationBuffer->getUsage() & BufferUsage::TRANSFER_DST)) {
					ENGINE_ERROR("destinationBuffer must have the TRANSFER_DST usage flag.");

					errorCount += 1;
				}
			}

			if (sourceBuffer != nullptr && destinationBuffer != nullptr) {
				uint32 copyRegionIndex = 0;

				while (copyRegionIndex < copyRegions.count()) {
					const BufferCopyRegion& copyRegion = copyRegions[copyRegionIndex];

					if (copyRegion.sourceOffset >= sourceBuffer->getSize()) {
						ENGINE_ERROR("descriptor.copyRegions[[]].sourceOffset must be less than the size of sourceBuffer.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.size == 0) {
						ENGINE_ERROR("descriptor.copyRegions[[]].size must not be 0.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.destinationOffset >= destinationBuffer->getSize()) {
						ENGINE_ERROR("descriptor.copyRegions[[]].destinationOffset must be less than the size of destinationBuffer.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.sourceOffset + copyRegion.size > sourceBuffer->getSize()) {
						ENGINE_ERROR("descriptor.copyRegions[[]] exceeds the bounds of sourceBuffer.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.destinationOffset + copyRegion.size > destinationBuffer->getSize()) {
						ENGINE_ERROR("descriptor.copyRegions[[]] exceeds the bounds of destinationBuffer.", copyRegionIndex);

						errorCount += 1;
					}

					copyRegionIndex += 1;
				}
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->copyBufferToBufferImplementation(this, sourceBuffer, destinationBuffer, copyRegions);
	}

	void CommandEncoder::copyBufferToImage(ref<Buffer> sourceBuffer, ref<Image> destinationImage, view<BufferImageCopyRegion> copyRegions) {
		// TODO(graphics/validation)

		device->adapter->copyBufferToImageImplementation(this, sourceBuffer, destinationImage, copyRegions);
	}

	void CommandEncoder::copyImageToBuffer(ref<Image> sourceImage, ref<Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (sourceImage == nullptr) {
				ENGINE_ERROR("sourceImage must be a valid Image object.");

				errorCount += 1;
			}
			else {
				if (!(sourceImage->getUsage() & ImageUsage::TRANSFER_SRC)) {
					ENGINE_ERROR("sourceImage must have the TRANSFER_SRC usage flag.");

					errorCount += 1;
				}
			}

			if (destinationBuffer == nullptr) {
				ENGINE_ERROR("destinationBuffer must be a valid Buffer object.");

				errorCount += 1;
			}
			else {
				if (!(destinationBuffer->getUsage() & BufferUsage::TRANSFER_DST)) {
					ENGINE_ERROR("destinationBuffer must have the TRANSFER_DST usage flag.");

					errorCount += 1;
				}
			}

			if (sourceImage != nullptr && destinationBuffer != nullptr) {
				uint32 copyRegionIndex = 0;

				while (copyRegionIndex < copyRegions.count()) {
					const BufferImageCopyRegion& copyRegion = copyRegions[copyRegionIndex];
					const uint64 copyRegionSize = copyRegion.imageExtent.width * copyRegion.imageExtent.height * copyRegion.imageExtent.depth * copyRegion.imageLayerCount * getByteSize(sourceImage->getFormat());

					if (copyRegion.bufferOffset >= destinationBuffer->getSize()) {
						ENGINE_ERROR("descriptor.copyRegions[[]].bufferOffset must be less than the size of destinationBuffer.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegionSize > destinationBuffer->getSize() - copyRegion.bufferOffset) {
						ENGINE_ERROR("The area formed by descriptor.copyRegions[[]] must be less than or equal to the size of destinationBuffer.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.bufferOffset >= destinationBuffer->getSize()) {
						ENGINE_ERROR("descriptor.copyRegions[[]].bufferOffset must be less than the size of destinationBuffer.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.imageOffsetX >= static_cast<sint32>(sourceImage->getExtent().width)) {
						ENGINE_ERROR("descriptor.copyRegions[[]].imageOffsetX must be less than the width of sourceImage.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.imageOffsetY >= static_cast<sint32>(sourceImage->getExtent().height)) {
						ENGINE_ERROR("descriptor.copyRegions[[]].imageOffsetY must be less than the height of sourceImage.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.imageOffsetZ >= static_cast<sint32>(sourceImage->getExtent().depth)) {
						ENGINE_ERROR("descriptor.copyRegions[[]].imageOffsetZ must be less than the depth of sourceImage.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.imageExtent.width == 0) {
						ENGINE_ERROR("descriptor.copyRegions[[]].imageExtent.width must be greater than 0.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.imageExtent.height == 0) {
						ENGINE_ERROR("descriptor.copyRegions[[]].imageExtent.height must be greater than 0.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.imageExtent.depth == 0) {
						ENGINE_ERROR("descriptor.copyRegions[[]].imageExtent.depth must be greater than 0.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.imageOffsetX + copyRegion.imageExtent.width > sourceImage->getExtent().width) {
						ENGINE_ERROR("The width formed by descriptor.copyRegions[[]] must be less than or equal to the width of sourceImage.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.imageOffsetY + copyRegion.imageExtent.height > sourceImage->getExtent().height) {
						ENGINE_ERROR("The height formed by descriptor.copyRegions[[]] must be less than or equal to the height of sourceImage.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.imageOffsetZ + copyRegion.imageExtent.depth > sourceImage->getExtent().depth) {
						ENGINE_ERROR("The depth formed by descriptor.copyRegions[[]] must be less than or equal to the depth of sourceImage.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.imageMipLevel >= sourceImage->getMipLevelCount()) {
						ENGINE_ERROR("descriptor.copyRegions[[]].imageMipLevel must be less than the number of mip levels in the source image.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.imageLayerOffset >= sourceImage->getLayerCount()) {
						ENGINE_ERROR("descriptor.copyRegions[[]].imageLayerOffset must be less than the number of layers in the source image.", copyRegionIndex);

						errorCount += 1;
					}

					if (copyRegion.imageLayerCount == 0) {
						ENGINE_ERROR("descriptor.copyRegions[[]].imageLayerCount must be greater than 0.", copyRegionIndex);

						errorCount += 1;
					}

					copyRegionIndex += 1;
				}
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->copyImageToBufferImplementation(this, sourceImage, destinationBuffer, copyRegions);
	}

	void CommandEncoder::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			const vec3<uint32> maxComputeWorkgroupCount = device->getPhysicalDevice()->getLimits().maxComputeWorkgroupCount;

			uint32 errorCount = 0;

			if (groupCountX > maxComputeWorkgroupCount[0]) {
				ENGINE_ERROR("The group count in the X axis must not exceed [].", maxComputeWorkgroupCount[0]);

				errorCount += 1;
			}

			if (groupCountX > maxComputeWorkgroupCount[1]) {
				ENGINE_ERROR("The group count in the Y axis must not exceed [].", maxComputeWorkgroupCount[1]);

				errorCount += 1;
			}

			if (groupCountX > maxComputeWorkgroupCount[2]) {
				ENGINE_ERROR("The group count in the Z axis must not exceed [].", maxComputeWorkgroupCount[2]);

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->dispatchImplementation(this, groupCountX, groupCountY, groupCountZ);
	}

	void CommandEncoder::dispatchIndirect(ref<Buffer> buffer, uint64 bufferOffset) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (buffer == nullptr) {
				ENGINE_ERROR("buffer must be a valid Buffer object.");

				errorCount += 1;
			}
			else {
				if (!(buffer->getUsage() & BufferUsage::INDIRECT)) {
					ENGINE_ERROR("buffer must have the INDIRECT usage flag.");

					errorCount += 1;
				}

				if (bufferOffset >= buffer->getSize()) {
					ENGINE_ERROR("bufferOffset exceeds the bounds of buffer.");

					errorCount += 1;
				}
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->dispatchIndirectImplementation(this, buffer, bufferOffset);
	}

	void CommandEncoder::draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (vertexCount == 0) {
				ENGINE_WARNING("vertexCount should not be 0.");
			}

			if (instanceCount == 0) {
				ENGINE_WARNING("instanceCount should not be 0.");
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->drawImplementation(this, vertexCount, instanceCount, vertexOffset, instanceOffset);
	}

	void CommandEncoder::drawIndirect(ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (buffer == nullptr) {
				ENGINE_ERROR("buffer must be a valid Buffer object.");

				errorCount += 1;
			}
			else {
				if (!(buffer->getUsage() & BufferUsage::INDIRECT)) {
					ENGINE_ERROR("buffer must have the INDIRECT usage flag.");

					errorCount += 1;
				}

				if (bufferOffset >= buffer->getSize()) {
					ENGINE_ERROR("bufferOffset exceeds the bounds of buffer.");

					errorCount += 1;
				}
			}

			if (drawCount == 0) {
				ENGINE_ERROR("drawCount must not be 0.");

				errorCount += 1;
			}

			if (stride == 0) {
				ENGINE_ERROR("stride must not be 0.");

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->drawIndirectImplementation(this, buffer, bufferOffset, drawCount, stride);
	}

	void CommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (indexCount == 0) {
				ENGINE_WARNING("indexCount should not be 0.");
			}

			if (instanceCount == 0) {
				ENGINE_WARNING("instanceCount should not be 0.");
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->drawIndexedImplementation(this, indexCount, instanceCount, indexOffset, indexBias, instanceOffset);
	}

	void CommandEncoder::drawIndexedIndirect(ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (buffer == nullptr) {
				ENGINE_ERROR("buffer must be a valid Buffer object.");

				errorCount += 1;
			}
			else {
				if (!(buffer->getUsage() & BufferUsage::INDIRECT)) {
					ENGINE_ERROR("buffer must have the INDIRECT usage flag.");

					errorCount += 1;
				}

				if (bufferOffset >= buffer->getSize()) {
					ENGINE_ERROR("bufferOffset exceeds the bounds of buffer.");

					errorCount += 1;
				}
			}

			if (drawCount == 0) {
				ENGINE_ERROR("drawCount must not be 0.");

				errorCount += 1;
			}

			if (stride == 0) {
				ENGINE_ERROR("stride must not be 0.");

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->drawIndexedIndirectImplementation(this, buffer, bufferOffset, drawCount, stride);
	}

	ref<CommandBuffer> CommandEncoder::end() const {
		return device->adapter->endImplementation(this);
	}

	void CommandEncoder::endRenderPass() {
		device->adapter->endRenderPassImplementation(this);

		const ref<RenderPass> renderPass = currentFramebuffer->getRenderPass();
		const view<ref<ImageView>> imageViews = currentFramebuffer->getImageViews();

		uint32 attachmentIndex = 0;

		while (attachmentIndex < renderPass->getAttachmentCount()) {
			const Attachment& attachment = renderPass->getAttachment(attachmentIndex);
			const ref<ImageView> imageView = imageViews[attachmentIndex];

			imageView->getImage()->layout = attachment.finalLayout;

			attachmentIndex += 1;
		}
	}

	void CommandEncoder::nextSubpass() {
		device->adapter->nextSubpassImplementation(this);
	}

	void CommandEncoder::insertMemoryBarriers(view<BufferMemoryBarrier> bufferMemoryBarriers, view<ImageMemoryBarrier> imageMemoryBarriers) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;
			uint32 bufferMemoryBarrierIndex = 0;
			uint32 imageMemoryBarrierIndex = 0;

			while (bufferMemoryBarrierIndex < bufferMemoryBarriers.count()) {
				const BufferMemoryBarrier& bufferMemoryBarrier = bufferMemoryBarriers[bufferMemoryBarrierIndex];

				if (bufferMemoryBarrier.buffer == nullptr) {
					ENGINE_ERROR("bufferMemoryBarriers[[]].buffer must be a valid BufferMemoryBarrier object.", bufferMemoryBarrierIndex);

					errorCount += 1;
				}

				bufferMemoryBarrierIndex += 1;
			}

			while (imageMemoryBarrierIndex < imageMemoryBarriers.count()) {
				const ImageMemoryBarrier& imageMemoryBarrier = imageMemoryBarriers[imageMemoryBarrierIndex];

				if (imageMemoryBarrier.image != nullptr) {
					if (imageMemoryBarrier.newLayout == imageMemoryBarrier.image->getLayout()) {
						ENGINE_WARNING("imageMemoryBarriers[[]].newLayout is the same as the current image layout.", imageMemoryBarrierIndex);
					}
				}
				else {
					ENGINE_ERROR("imageMemoryBarriers[[]].image must be a valid Image object.", imageMemoryBarrierIndex);

					errorCount += 1;
				}

				imageMemoryBarrierIndex += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		device->adapter->insertMemoryBarriersImplementation(this, bufferMemoryBarriers, imageMemoryBarriers);
	}

	void CommandEncoder::reset() {
		device->adapter->resetImplementation(this);
	}

	void CommandEncoder::setLineWidth(float32 lineWidth) {
		device->adapter->setLineWidthImplementation(this, lineWidth);
	}

	void CommandEncoder::setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) {
		device->adapter->setScissorImplementation(this, xOffset, yOffset, extent);
	}

	void CommandEncoder::setViewport(vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) {
		device->adapter->setViewportImplementation(this, origin, size, minDepth, maxDepth);
	}
}