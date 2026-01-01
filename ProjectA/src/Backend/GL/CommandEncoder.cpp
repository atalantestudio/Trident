#include "pch.hpp"

#include "Backend/GL/Command/BeginRenderPassCommand.hpp"
#include "Backend/GL/Command/BindComputePipelineCommand.hpp"
#include "Backend/GL/Command/BindDescriptorSetsCommand.hpp"
#include "Backend/GL/Command/BindIndexBufferCommand.hpp"
#include "Backend/GL/Command/BindRenderPipelineCommand.hpp"
#include "Backend/GL/Command/BindVertexBufferCommand.hpp"
#include "Backend/GL/Command/CopyBufferToBufferCommand.hpp"
#include "Backend/GL/Command/CopyBufferToImageCommand.hpp"
#include "Backend/GL/Command/CopyImageToBufferCommand.hpp"
#include "Backend/GL/Command/DispatchCommand.hpp"
#include "Backend/GL/Command/DispatchIndirectCommand.hpp"
#include "Backend/GL/Command/DrawCommand.hpp"
#include "Backend/GL/Command/DrawIndexedCommand.hpp"
#include "Backend/GL/Command/DrawIndexedIndirectCommand.hpp"
#include "Backend/GL/Command/DrawIndirectCommand.hpp"
#include "Backend/GL/Command/EndRenderPassCommand.hpp"
#include "Backend/GL/Command/NextSubpassCommand.hpp"
#include "Backend/GL/Command/SetImageMemoryBarrierCommand.hpp"
#include "Backend/GL/Command/SetLineWidthCommand.hpp"
#include "Backend/GL/Command/SetScissorCommand.hpp"
#include "Backend/GL/Command/SetViewportCommand.hpp"
#include "Backend/GL/CommandBuffer.hpp"
#include "Backend/GL/CommandEncoder.hpp"
#include "Backend/GL/Image.hpp"

#include "Backend/GL/ImageLayout.hpp"
#include "Backend/GL/IndexType.hpp"

namespace ProjectA {
	OpenGLCommandEncoder::OpenGLCommandEncoder(const OpenGLDevice& device, const CommandBufferDescriptor& descriptor) :
		limits(device.getPhysicalDevice()->getLimits())
	{}

	void OpenGLCommandEncoder::begin() const {}

	void OpenGLCommandEncoder::beginRenderPass(const BeginRenderPassDescriptor& descriptor) {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		framebuffer = std::static_pointer_cast<OpenGLFramebuffer>(descriptor.framebuffer);
		renderPass = std::static_pointer_cast<OpenGLRenderPass>(framebuffer->getRenderPass());

		commandBuffer.emplace_back(createRef<OpenGLBeginRenderPassCommand>(descriptor));
	}

	void OpenGLCommandEncoder::bindComputePipeline(const ref<ComputePipeline>& computePipeline) {
		this->computePipeline = std::static_pointer_cast<OpenGLComputePipeline>(computePipeline);

		commandBuffer.emplace_back(createRef<OpenGLBindComputePipelineCommand>(this->computePipeline));
	}

	void OpenGLCommandEncoder::bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) {
		this->renderPipeline = std::static_pointer_cast<OpenGLRenderPipeline>(renderPipeline);

		ENGINE_ASSERT(this->renderPipeline->subpassIndex == subpassIndex, "The pipeline must be associated to the current subpass.");

		commandBuffer.emplace_back(createRef<OpenGLBindRenderPipelineCommand>(this->renderPipeline));
	}

	void OpenGLCommandEncoder::bindDescriptorSets(const ref<PipelineLayout>& pipelineLayout, const sequence<ref<DescriptorSet>>& descriptorSets) {
		commandBuffer.emplace_back(createRef<OpenGLBindDescriptorSetsCommand>(descriptorSets));
	}

	void OpenGLCommandEncoder::bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset, uint8 indexTypeSize) {
		indexOffset = static_cast<uint32>(offset);
		indexType = getGlIndexType(indexTypeSize);
		this->indexTypeSize = indexTypeSize;

		commandBuffer.emplace_back(createRef<OpenGLBindIndexBufferCommand>(
			renderPipeline->vaobj,
			std::static_pointer_cast<OpenGLBuffer>(indexBuffer)->object
		));
	}

	void OpenGLCommandEncoder::bindVertexBuffer(uint32 bindingIndex, const ref<Buffer>& vertexBuffer, uint64 offset, uint32 stride) {
		commandBuffer.emplace_back(createRef<OpenGLBindVertexBufferCommand>(
			renderPipeline->vaobj,
			bindingIndex,
			std::static_pointer_cast<OpenGLBuffer>(vertexBuffer)->object,
			offset,
			stride
		));
	}

	void OpenGLCommandEncoder::copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) {
		commandBuffer.emplace_back(createRef<OpenGLCopyBufferToBufferCommand>(
			std::static_pointer_cast<OpenGLBuffer>(sourceBuffer)->object,
			std::static_pointer_cast<OpenGLBuffer>(destinationBuffer)->object,
			copyRegions
		));
	}

	void OpenGLCommandEncoder::copyBufferToImage(const ref<Buffer>& sourceBuffer, const ref<Image>& destinationImage, const sequence<BufferImageCopyRegion>& copyRegions) {
		commandBuffer.emplace_back(createRef<OpenGLCopyBufferToImageCommand>(
			std::static_pointer_cast<OpenGLBuffer>(sourceBuffer)->object,
			std::static_pointer_cast<OpenGLImage>(destinationImage).get(),
			copyRegions
		));
	}

	void OpenGLCommandEncoder::copyImageToBuffer(const ref<Image>& sourceImage, const ref<Buffer>& destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) {
		commandBuffer.emplace_back(createRef<OpenGLCopyImageToBufferCommand>(
			std::static_pointer_cast<OpenGLImage>(sourceImage).get(),
			std::static_pointer_cast<OpenGLBuffer>(destinationBuffer).get(),
			copyRegions
		));
	}

	void OpenGLCommandEncoder::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		ENGINE_ASSERT(groupCountX <= limits.maxComputeWorkgroupCount[0], "Group count in the X axis must not exceed [].", limits.maxComputeWorkgroupCount[0]);
		ENGINE_ASSERT(groupCountY <= limits.maxComputeWorkgroupCount[1], "Group count in the Y axis must not exceed [].", limits.maxComputeWorkgroupCount[1]);
		ENGINE_ASSERT(groupCountZ <= limits.maxComputeWorkgroupCount[2], "Group count in the Z axis must not exceed [].", limits.maxComputeWorkgroupCount[2]);

		commandBuffer.emplace_back(createRef<OpenGLDispatchCommand>(groupCountX, groupCountY, groupCountZ));
	}

	void OpenGLCommandEncoder::dispatchIndirect(const ref<Buffer>& buffer, uint64 offset) {
		commandBuffer.emplace_back(createRef<OpenGLDispatchIndirectCommand>(std::static_pointer_cast<OpenGLBuffer>(buffer)->object, offset));
	}

	void OpenGLCommandEncoder::nextSubpass() {
		subpassIndex += 1;

		ENGINE_ASSERT(subpassIndex < renderPass->getSubpassCount(), "All subpasses have been executed.");

		commandBuffer.emplace_back(createRef<OpenGLNextSubpassCommand>(framebuffer, renderPass->getSubpass(subpassIndex)));
	}

	void OpenGLCommandEncoder::setImageMemoryBarrier(const ref<Image>& image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) {
		const GLbitfield barrier = getGlBarrier(image->getLayout());

		if (barrier == 0) {
			return;
		}

		commandBuffer.emplace_back(createRef<OpenGLSetImageMemoryBarrierCommand>(barrier));

		std::static_pointer_cast<OpenGLImage>(image)->setLayout(newLayout);
	}

	void OpenGLCommandEncoder::setLineWidth(float32 lineWidth) {
		commandBuffer.emplace_back(createRef<OpenGLSetLineWidthCommand>(lineWidth));
	}

	void OpenGLCommandEncoder::setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) {
		commandBuffer.emplace_back(createRef<OpenGLSetScissorCommand>(
			xOffset,
			yOffset,
			extent
		));
	}

	void OpenGLCommandEncoder::setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) {
		commandBuffer.emplace_back(createRef<OpenGLSetViewportCommand>(
			static_cast<GLint>(origin[0]),

			// TODO: Allow the origin's Y to be flipped like in Vulkan.
			0,

			static_cast<GLsizei>(abs(size[0])),
			static_cast<GLsizei>(abs(size[1]))
		));
	}

	void OpenGLCommandEncoder::draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		commandBuffer.emplace_back(createRef<OpenGLDrawCommand>(
			renderPipeline->primitiveTopology,
			vertexOffset,
			vertexCount,
			instanceCount,
			instanceOffset
		));
	}

	void OpenGLCommandEncoder::drawIndirect(const ref<Buffer>& buffer, uint64 offset, uint32 drawCount, uint32 stride) {
		ENGINE_ASSERT(buffer->usage & BufferUsage::INDIRECT, "This buffer cannot be used as an indirect buffer.");
		ENGINE_ASSERT(drawCount > 0, "A DrawIndirect command must result in at least 1 draw call.");

		commandBuffer.emplace_back(createRef<OpenGLDrawIndirectCommand>(
			renderPipeline->primitiveTopology,
			std::static_pointer_cast<OpenGLBuffer>(buffer)->object,
			(const GLubyte*) offset,
			drawCount,
			stride
		));
	}

	void OpenGLCommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		const uint64 indices = this->indexOffset + indexOffset * indexTypeSize;

		commandBuffer.emplace_back(createRef<OpenGLDrawIndexedCommand>(
			renderPipeline->primitiveTopology,
			indexCount,
			indexType,
			(const GLvoid*) indices,
			instanceCount,
			indexBias,
			instanceOffset
		));
	}

	void OpenGLCommandEncoder::drawIndexedIndirect(const ref<Buffer>& buffer, uint64 offset, uint32 drawCount, uint32 stride) {
		ENGINE_ASSERT(buffer->usage & BufferUsage::INDIRECT, "This buffer cannot be used as an indirect buffer.");
		ENGINE_ASSERT(drawCount > 0, "A DrawIndirect command must result in at least 1 draw call.");

		commandBuffer.emplace_back(createRef<OpenGLDrawIndexedIndirectCommand>(
			renderPipeline->primitiveTopology,
			indexType,
			std::static_pointer_cast<OpenGLBuffer>(buffer)->object,
			(const GLubyte*) offset,
			drawCount,
			stride
		));
	}

	void OpenGLCommandEncoder::endRenderPass() {
		commandBuffer.emplace_back(createRef<OpenGLEndRenderPassCommand>(framebuffer));
	}

	ref<CommandBuffer> OpenGLCommandEncoder::end() const {
		return createRef<OpenGLCommandBuffer>(commandBuffer);
	}

	void OpenGLCommandEncoder::reset() {
		commandBuffer.clear();

		framebuffer = nullptr;
		renderPass = nullptr;
		subpassIndex = 0;
		computePipeline = nullptr;
		renderPipeline = nullptr;
		indexOffset = 0;
		indexType = 0;
		indexTypeSize = 0;
	}
}