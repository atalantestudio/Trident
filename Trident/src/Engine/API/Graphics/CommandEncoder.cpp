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
			{
				Validator validator;

				if (descriptor.framebuffer == nullptr) {
					VALIDATION_ERROR("descriptor.framebuffer must be a valid Framebuffer object.");
				}
				else {
					const ref<RenderPass> renderPass = descriptor.framebuffer->getRenderPass();

					if (renderPass == nullptr) {
						VALIDATION_ERROR("descriptor.framebuffer.renderPass must be a valid RenderPass object.");
					}
					else {
						if (descriptor.framebuffer->getImageViews().count() != renderPass->getTotalAttachmentCount()) {
							VALIDATION_ERROR("descriptor.framebuffer must have one ImageView object for each attachment in the render pass. This includes the implicit resolve attachment for the swapchain image.");
						}

						if (descriptor.clearValues.count() < renderPass->getAttachmentCount()) {
							VALIDATION_ERROR("descriptor.clearValues.count must be greater than or equal to the number of attachments in descriptor.framebuffer.renderPass.");
						}

						// TODO(graphics/validation-layer): Restore.
						/* if (descriptor.clearValues.count() > renderPass->getAttachmentReferenceCount()) {
							VALIDATION_ERROR("descriptor.clearValues.count must be smaller than the number of attachment references in descriptor.framebuffer.renderPass.");
						} */
					}
				}
			}
		#endif

		device->adapter->beginRenderPassImplementation(this, descriptor);

		currentFramebuffer = descriptor.framebuffer;
	}

	void CommandEncoder::bindComputePipeline(ref<ComputePipeline> computePipeline) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (computePipeline == nullptr) {
					VALIDATION_ERROR("computePipeline must be a valid ComputePipeline object.");
				}
			}
		#endif

		device->adapter->bindComputePipelineImplementation(this, computePipeline);
	}

	void CommandEncoder::bindDescriptorSets(ref<PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<DescriptorSet>> descriptorSets) {
		// TODO(graphics/validation/null-object-guard)

		device->adapter->bindDescriptorSetsImplementation(this, pipelineLayout, descriptorSetIndexOffset, descriptorSets);
	}

	void CommandEncoder::bindGraphicsPipeline(ref<GraphicsPipeline> graphicsPipeline) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (graphicsPipeline == nullptr) {
					VALIDATION_ERROR("graphicsPipeline must be a valid GraphicsPipeline object.");
				}
			}
		#endif

		device->adapter->bindGraphicsPipelineImplementation(this, graphicsPipeline);
	}

	void CommandEncoder::bindIndexBuffer(ref<Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (indexBuffer == nullptr) {
					VALIDATION_ERROR("indexBuffer must be a valid Buffer object.");
				}
				else {
					if (!(indexBuffer->getUsage() & BufferUsage::INDEX)) {
						VALIDATION_ERROR("indexBuffer must have the INDEX usage flag.");
					}

					if (offset >= indexBuffer->getSize()) {
						VALIDATION_ERROR("bufferOffset must be less than the size of indexBuffer.");
					}
				}

				if (indexTypeSize == 0) {
					VALIDATION_ERROR("The index size must not be 0.");
				}
			}
		#endif

		device->adapter->bindIndexBufferImplementation(this, indexBuffer, offset, indexTypeSize);
	}

	void CommandEncoder::bindVertexBuffer(uint32 bindingIndex, ref<Buffer> vertexBuffer, uint64 offset, uint32 stride) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				// TODO(graphics/command-encoder/bind-vertex-buffer): Validate the binding index.

				if (vertexBuffer == nullptr) {
					VALIDATION_ERROR("vertexBuffer must be a valid Buffer object.");
				}
				else {
					if (!(vertexBuffer->getUsage() & BufferUsage::VERTEX)) {
						VALIDATION_ERROR("vertexBuffer must have the VERTEX usage flag.");
					}

					if (offset >= vertexBuffer->getSize()) {
						VALIDATION_ERROR("bufferOffset must be less than the size of vertexBuffer.");
					}
				}

				if (stride == 0) {
					VALIDATION_ERROR("stride must not be 0.");
				}
			}
		#endif

		device->adapter->bindVertexBufferImplementation(this, bindingIndex, vertexBuffer, offset, stride);
	}

	void CommandEncoder::copyBufferToBuffer(ref<Buffer> sourceBuffer, ref<Buffer> destinationBuffer, view<BufferCopyRegion> copyRegions) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (sourceBuffer == nullptr) {
					VALIDATION_ERROR("sourceBuffer must be a valid Buffer object.");
				}
				else {
					if (!(sourceBuffer->getUsage() & BufferUsage::TRANSFER_SRC)) {
						VALIDATION_ERROR("sourceBuffer must have the TRANSFER_SRC usage flag.");
					}
				}

				if (destinationBuffer == nullptr) {
					VALIDATION_ERROR("destinationBuffer must be a valid Buffer object.");
				}
				else {
					if (!(destinationBuffer->getUsage() & BufferUsage::TRANSFER_DST)) {
						VALIDATION_ERROR("destinationBuffer must have the TRANSFER_DST usage flag.");
					}
				}

				if (sourceBuffer != nullptr && destinationBuffer != nullptr) {
					uint32 copyRegionIndex = 0;

					while (copyRegionIndex < copyRegions.count()) {
						const BufferCopyRegion& copyRegion = copyRegions[copyRegionIndex];

						if (copyRegion.sourceOffset >= sourceBuffer->getSize()) {
							VALIDATION_ERROR("copyRegions[[]].sourceOffset must be less than the size of sourceBuffer.", copyRegionIndex);
						}

						if (copyRegion.destinationOffset >= destinationBuffer->getSize()) {
							VALIDATION_ERROR("copyRegions[[]].destinationOffset must be less than the size of destinationBuffer.", copyRegionIndex);
						}

						if (copyRegion.size > sourceBuffer->getSize() - copyRegion.sourceOffset) {
							VALIDATION_ERROR("copyRegions[[]].size must be less than or equal to the size of sourceBuffer - copyRegions[[]].sourceOffset.", copyRegionIndex, copyRegionIndex);
						}

						if (copyRegion.size > destinationBuffer->getSize() - copyRegion.destinationOffset) {
							VALIDATION_ERROR("copyRegions[[]].size must be less than or equal to the size of destinationBuffer - copyRegions[[]].destinationOffset.", copyRegionIndex, copyRegionIndex);
						}

						// TODO(graphics/validation)
						// "The union of the source regions, and the union of the destination regions, specified by the elements of pRegions, must not overlap in memory."
						// (VUID-vkCmdCopyBuffer-pRegions-00117)

						copyRegionIndex += 1;
					}
				}
			}
		#endif

		device->adapter->copyBufferToBufferImplementation(this, sourceBuffer, destinationBuffer, copyRegions);
	}

	void CommandEncoder::copyBufferToImage(ref<Buffer> sourceBuffer, ref<Image> destinationImage, view<BufferImageCopyRegion> copyRegions) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (sourceBuffer == nullptr) {
					VALIDATION_ERROR("sourceBuffer must be a valid Buffer object.");
				}
				else {
					if (!(sourceBuffer->getUsage() & BufferUsage::TRANSFER_SRC)) {
						VALIDATION_ERROR("sourceBuffer must have the TRANSFER_SRC usage flag.");
					}
				}

				if (destinationImage == nullptr) {
					VALIDATION_ERROR("destinationImage must be a valid Image object.");
				}
				else {
					if (!(destinationImage->getUsage() & ImageUsage::TRANSFER_DST)) {
						VALIDATION_ERROR("destinationImage must have the TRANSFER_DST usage flag.");
					}
				}

				if (sourceBuffer != nullptr && destinationImage != nullptr) {
					uint32 copyRegionIndex = 0;

					while (copyRegionIndex < copyRegions.count()) {
						const BufferImageCopyRegion& copyRegion = copyRegions[copyRegionIndex];
						const uint64 copyRegionSize = copyRegion.imageExtent.width * copyRegion.imageExtent.height * copyRegion.imageExtent.depth * copyRegion.imageLayerCount * getByteSize(destinationImage->getFormat());

						if (copyRegion.bufferOffset >= sourceBuffer->getSize()) {
							VALIDATION_ERROR("copyRegions[[]].bufferOffset must be less than the size of sourceBuffer.", copyRegionIndex);
						}

						if (copyRegionSize > sourceBuffer->getSize() - copyRegion.bufferOffset) {
							VALIDATION_ERROR("The area formed by copyRegions[[]] must be less than or equal to the size of sourceBuffer - copyRegions[[]].bufferOffset.", copyRegionIndex, copyRegionIndex);
						}

						if (copyRegion.imageOffsetX >= static_cast<sint32>(destinationImage->getExtent().width)) {
							VALIDATION_ERROR("copyRegions[[]].imageOffsetX must be less than the width of destinationImage.", copyRegionIndex);
						}

						if (copyRegion.imageOffsetY >= static_cast<sint32>(destinationImage->getExtent().height)) {
							VALIDATION_ERROR("copyRegions[[]].imageOffsetY must be less than the height of destinationImage.", copyRegionIndex);
						}

						if (copyRegion.imageOffsetZ >= static_cast<sint32>(destinationImage->getExtent().depth)) {
							VALIDATION_ERROR("copyRegions[[]].imageOffsetZ must be less than the depth of destinationImage.", copyRegionIndex);
						}

						if (copyRegion.imageExtent.width == 0) {
							VALIDATION_ERROR("copyRegions[[]].imageExtent.width must be greater than 0.", copyRegionIndex);
						}

						if (copyRegion.imageExtent.height == 0) {
							VALIDATION_ERROR("copyRegions[[]].imageExtent.height must be greater than 0.", copyRegionIndex);
						}

						if (copyRegion.imageExtent.depth == 0) {
							VALIDATION_ERROR("copyRegions[[]].imageExtent.depth must be greater than 0.", copyRegionIndex);
						}

						if (copyRegion.imageOffsetX + copyRegion.imageExtent.width > destinationImage->getExtent().width) {
							VALIDATION_ERROR("The width formed by copyRegions[[]] must be less than or equal to the width of destinationImage.", copyRegionIndex);
						}

						if (copyRegion.imageOffsetY + copyRegion.imageExtent.height > destinationImage->getExtent().height) {
							VALIDATION_ERROR("The height formed by copyRegions[[]] must be less than or equal to the height of destinationImage.", copyRegionIndex);
						}

						if (copyRegion.imageOffsetZ + copyRegion.imageExtent.depth > destinationImage->getExtent().depth) {
							VALIDATION_ERROR("The depth formed by copyRegions[[]] must be less than or equal to the depth of destinationImage.", copyRegionIndex);
						}

						if (copyRegion.imageMipLevel >= destinationImage->getMipLevelCount()) {
							VALIDATION_ERROR("copyRegions[[]].imageMipLevel must be less than the number of mip levels in destinationImage.", copyRegionIndex);
						}

						if (copyRegion.imageLayerOffset >= destinationImage->getLayerCount()) {
							VALIDATION_ERROR("copyRegions[[]].imageLayerOffset must be less than the number of layers in destinationImage.", copyRegionIndex);
						}

						if (copyRegion.imageLayerCount == 0) {
							VALIDATION_ERROR("copyRegions[[]].imageLayerCount must be greater than 0.", copyRegionIndex);
						}

						if (copyRegion.imageLayerCount > destinationImage->getLayerCount() - copyRegion.imageLayerOffset) {
							VALIDATION_ERROR("copyRegions[[]].imageLayerCount must be less than or equal to the number of layers in destinationImage - copyRegions[[]].imageLayerOffset.", copyRegionIndex, copyRegionIndex);
						}

						copyRegionIndex += 1;
					}
				}
			}
		#endif

		device->adapter->copyBufferToImageImplementation(this, sourceBuffer, destinationImage, copyRegions);
	}

	void CommandEncoder::copyImageToBuffer(ref<Image> sourceImage, ref<Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (sourceImage == nullptr) {
					VALIDATION_ERROR("sourceImage must be a valid Image object.");
				}
				else {
					if (!(sourceImage->getUsage() & ImageUsage::TRANSFER_SRC)) {
						VALIDATION_ERROR("sourceImage must have the TRANSFER_SRC usage flag.");
					}
				}

				if (destinationBuffer == nullptr) {
					VALIDATION_ERROR("destinationBuffer must be a valid Buffer object.");
				}
				else {
					if (!(destinationBuffer->getUsage() & BufferUsage::TRANSFER_DST)) {
						VALIDATION_ERROR("destinationBuffer must have the TRANSFER_DST usage flag.");
					}
				}

				if (sourceImage != nullptr && destinationBuffer != nullptr) {
					uint32 copyRegionIndex = 0;

					while (copyRegionIndex < copyRegions.count()) {
						const BufferImageCopyRegion& copyRegion = copyRegions[copyRegionIndex];
						const uint64 copyRegionSize = copyRegion.imageExtent.width * copyRegion.imageExtent.height * copyRegion.imageExtent.depth * copyRegion.imageLayerCount * getByteSize(sourceImage->getFormat());

						if (copyRegion.bufferOffset >= destinationBuffer->getSize()) {
							VALIDATION_ERROR("copyRegions[[]].bufferOffset must be less than the size of destinationBuffer.", copyRegionIndex);
						}

						if (copyRegionSize > destinationBuffer->getSize() - copyRegion.bufferOffset) {
							VALIDATION_ERROR("The area formed by copyRegions[[]] must be less than or equal to the size of destinationBuffer - copyRegions[[]].bufferOffset.", copyRegionIndex, copyRegionIndex);
						}

						if (copyRegion.imageOffsetX >= static_cast<sint32>(sourceImage->getExtent().width)) {
							VALIDATION_ERROR("copyRegions[[]].imageOffsetX must be less than the width of sourceImage.", copyRegionIndex);
						}

						if (copyRegion.imageOffsetY >= static_cast<sint32>(sourceImage->getExtent().height)) {
							VALIDATION_ERROR("copyRegions[[]].imageOffsetY must be less than the height of sourceImage.", copyRegionIndex);
						}

						if (copyRegion.imageOffsetZ >= static_cast<sint32>(sourceImage->getExtent().depth)) {
							VALIDATION_ERROR("copyRegions[[]].imageOffsetZ must be less than the depth of sourceImage.", copyRegionIndex);
						}

						if (copyRegion.imageExtent.width == 0) {
							VALIDATION_ERROR("copyRegions[[]].imageExtent.width must be greater than 0.", copyRegionIndex);
						}

						if (copyRegion.imageExtent.height == 0) {
							VALIDATION_ERROR("copyRegions[[]].imageExtent.height must be greater than 0.", copyRegionIndex);
						}

						if (copyRegion.imageExtent.depth == 0) {
							VALIDATION_ERROR("copyRegions[[]].imageExtent.depth must be greater than 0.", copyRegionIndex);
						}

						if (copyRegion.imageOffsetX + copyRegion.imageExtent.width > sourceImage->getExtent().width) {
							VALIDATION_ERROR("The width formed by copyRegions[[]] must be less than or equal to the width of sourceImage.", copyRegionIndex);
						}

						if (copyRegion.imageOffsetY + copyRegion.imageExtent.height > sourceImage->getExtent().height) {
							VALIDATION_ERROR("The height formed by copyRegions[[]] must be less than or equal to the height of sourceImage.", copyRegionIndex);
						}

						if (copyRegion.imageOffsetZ + copyRegion.imageExtent.depth > sourceImage->getExtent().depth) {
							VALIDATION_ERROR("The depth formed by copyRegions[[]] must be less than or equal to the depth of sourceImage.", copyRegionIndex);
						}

						if (copyRegion.imageMipLevel >= sourceImage->getMipLevelCount()) {
							VALIDATION_ERROR("copyRegions[[]].imageMipLevel must be less than the number of mip levels in sourceImage.", copyRegionIndex);
						}

						if (copyRegion.imageLayerOffset >= sourceImage->getLayerCount()) {
							VALIDATION_ERROR("copyRegions[[]].imageLayerOffset must be less than the number of layers in sourceImage.", copyRegionIndex);
						}

						if (copyRegion.imageLayerCount == 0) {
							VALIDATION_ERROR("copyRegions[[]].imageLayerCount must be greater than 0.", copyRegionIndex);
						}

						if (copyRegion.imageLayerCount > sourceImage->getLayerCount() - copyRegion.imageLayerOffset) {
							VALIDATION_ERROR("copyRegions[[]].imageLayerCount must be less than or equal to the number of layers in sourceImage - copyRegions[[]].imageLayerOffset.", copyRegionIndex, copyRegionIndex);
						}

						copyRegionIndex += 1;
					}
				}
			}
		#endif

		device->adapter->copyImageToBufferImplementation(this, sourceImage, destinationBuffer, copyRegions);
	}

	void CommandEncoder::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				const vec3<uint32> maxComputeWorkgroupCount = device->getPhysicalDevice()->getLimits().maxComputeWorkgroupCount;

				Validator validator;

				if (groupCountX > maxComputeWorkgroupCount[0]) {
					VALIDATION_ERROR("The group count in the X axis must not exceed [].", maxComputeWorkgroupCount[0]);
				}

				if (groupCountX > maxComputeWorkgroupCount[1]) {
					VALIDATION_ERROR("The group count in the Y axis must not exceed [].", maxComputeWorkgroupCount[1]);
				}

				if (groupCountX > maxComputeWorkgroupCount[2]) {
					VALIDATION_ERROR("The group count in the Z axis must not exceed [].", maxComputeWorkgroupCount[2]);
				}
			}
		#endif

		device->adapter->dispatchImplementation(this, groupCountX, groupCountY, groupCountZ);
	}

	void CommandEncoder::dispatchIndirect(ref<Buffer> buffer, uint64 bufferOffset) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (buffer == nullptr) {
					VALIDATION_ERROR("buffer must be a valid Buffer object.");
				}
				else {
					if (!(buffer->getUsage() & BufferUsage::INDIRECT)) {
						VALIDATION_ERROR("buffer must have the INDIRECT usage flag.");
					}

					if (bufferOffset >= buffer->getSize()) {
						VALIDATION_ERROR("bufferOffset exceeds the bounds of buffer.");
					}
				}
			}
		#endif

		device->adapter->dispatchIndirectImplementation(this, buffer, bufferOffset);
	}

	void CommandEncoder::draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (vertexCount == 0) {
					VALIDATION_WARNING("vertexCount should not be 0.");
				}

				if (instanceCount == 0) {
					VALIDATION_WARNING("instanceCount should not be 0.");
				}
			}
		#endif

		device->adapter->drawImplementation(this, vertexCount, instanceCount, vertexOffset, instanceOffset);
	}

	void CommandEncoder::drawIndirect(ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (buffer == nullptr) {
					VALIDATION_ERROR("buffer must be a valid Buffer object.");
				}
				else {
					if (!(buffer->getUsage() & BufferUsage::INDIRECT)) {
						VALIDATION_ERROR("buffer must have the INDIRECT usage flag.");
					}

					if (bufferOffset >= buffer->getSize()) {
						VALIDATION_ERROR("bufferOffset exceeds the bounds of buffer.");
					}
				}

				if (drawCount == 0) {
					VALIDATION_ERROR("drawCount must not be 0.");
				}
			}
		#endif

		device->adapter->drawIndirectImplementation(this, buffer, bufferOffset, drawCount, stride);
	}

	void CommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (indexCount == 0) {
					VALIDATION_WARNING("indexCount should not be 0.");
				}

				if (instanceCount == 0) {
					VALIDATION_WARNING("instanceCount should not be 0.");
				}
			}
		#endif

		device->adapter->drawIndexedImplementation(this, indexCount, instanceCount, indexOffset, indexBias, instanceOffset);
	}

	void CommandEncoder::drawIndexedIndirect(ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (buffer == nullptr) {
					VALIDATION_ERROR("buffer must be a valid Buffer object.");
				}
				else {
					if (!(buffer->getUsage() & BufferUsage::INDIRECT)) {
						VALIDATION_ERROR("buffer must have the INDIRECT usage flag.");
					}

					if (bufferOffset >= buffer->getSize()) {
						VALIDATION_ERROR("bufferOffset exceeds the bounds of buffer.");
					}
				}

				if (drawCount == 0) {
					VALIDATION_ERROR("drawCount must not be 0.");
				}
			}
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
			{
				Validator validator;
				uint32 bufferMemoryBarrierIndex = 0;

				while (bufferMemoryBarrierIndex < bufferMemoryBarriers.count()) {
					const BufferMemoryBarrier& bufferMemoryBarrier = bufferMemoryBarriers[bufferMemoryBarrierIndex];

					if (bufferMemoryBarrier.buffer == nullptr) {
						VALIDATION_ERROR("bufferMemoryBarriers[[]].buffer must be a valid BufferMemoryBarrier object.", bufferMemoryBarrierIndex);
					}

					bufferMemoryBarrierIndex += 1;
				}

				uint32 imageMemoryBarrierIndex = 0;

				while (imageMemoryBarrierIndex < imageMemoryBarriers.count()) {
					const ImageMemoryBarrier& imageMemoryBarrier = imageMemoryBarriers[imageMemoryBarrierIndex];

					if (imageMemoryBarrier.image != nullptr) {
						if (imageMemoryBarrier.newLayout == imageMemoryBarrier.image->getLayout() && imageMemoryBarrier.newAccess == imageMemoryBarrier.image->getAccess()) {
							VALIDATION_WARNING("imageMemoryBarriers[[]] won't change the layout and access flags of the current image.", imageMemoryBarrierIndex);
						}
					}
					else {
						VALIDATION_ERROR("imageMemoryBarriers[[]].image must be a valid Image object.", imageMemoryBarrierIndex);
					}

					imageMemoryBarrierIndex += 1;
				}
			}
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