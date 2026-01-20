// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Command/BeginRenderPassCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/BindComputePipelineCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/BindDescriptorSetsCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/BindIndexBufferCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/BindRenderPipelineCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/BindVertexBufferCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/CopyBufferToBufferCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/CopyBufferToImageCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/CopyImageToBufferCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/DispatchCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/DispatchIndirectCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/DrawCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/DrawIndexedCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/DrawIndexedIndirectCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/DrawIndirectCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/EndRenderPassCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/NextSubpassCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/SetImageMemoryBarrierCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/SetLineWidthCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/SetScissorCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/SetViewportCommand.hpp"
#include "Engine/Adapter/Graphics/GL/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/GL/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageLayout.hpp"
#include "Engine/Adapter/Graphics/GL/IndexType.hpp"

namespace ProjectA {
	GL::CommandEncoder::CommandEncoder(const CommandBufferDescriptor& descriptor, const GL::Device& device) :
		limits(device.getPhysicalDevice()->getLimits())
	{
		DEBUG_DESCRIPTOR_GUARD(descriptor);
	}

	void GL::CommandEncoder::begin() const {}

	void GL::CommandEncoder::beginRenderPass(const BeginRenderPassDescriptor& descriptor) {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		framebuffer = std::static_pointer_cast<GL::Framebuffer>(descriptor.framebuffer);
		renderPass = std::static_pointer_cast<GL::RenderPass>(framebuffer->getRenderPass());

		commands.emplace_back(createRef<GL::BeginRenderPassCommand>(descriptor));
	}

	void GL::CommandEncoder::bindComputePipeline(ref<ProjectA::ComputePipeline> computePipeline) {
		this->computePipeline = std::static_pointer_cast<GL::ComputePipeline>(computePipeline);

		commands.emplace_back(createRef<GL::BindComputePipelineCommand>(this->computePipeline));
	}

	void GL::CommandEncoder::bindRenderPipeline(ref<ProjectA::RenderPipeline> renderPipeline) {
		this->renderPipeline = std::static_pointer_cast<GL::RenderPipeline>(renderPipeline);

		ADAPTER_ASSERT(this->renderPipeline->subpassIndex == subpassIndex, "The pipeline must be associated to the current subpass.");

		commands.emplace_back(createRef<GL::BindRenderPipelineCommand>(this->renderPipeline));
	}

	void GL::CommandEncoder::bindDescriptorSets(ref<PipelineLayout> pipelineLayout, const sequence<ref<ProjectA::DescriptorSet>>& descriptorSets) {
		commands.emplace_back(createRef<GL::BindDescriptorSetsCommand>(descriptorSets));
	}

	void GL::CommandEncoder::bindVertexBuffer(uint32 bindingIndex, ref<ProjectA::Buffer> vertexBuffer, uint64 offset, uint32 stride) {
		commands.emplace_back(createRef<GL::BindVertexBufferCommand>(
			renderPipeline->vertexArray,
			bindingIndex,
			std::static_pointer_cast<GL::Buffer>(vertexBuffer)->buffer,
			offset,
			stride
		));
	}

	void GL::CommandEncoder::copyBufferToBuffer(ref<ProjectA::Buffer> sourceBuffer, ref<ProjectA::Buffer> destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) {
		commands.emplace_back(createRef<GL::CopyBufferToBufferCommand>(
			std::static_pointer_cast<GL::Buffer>(sourceBuffer),
			std::static_pointer_cast<GL::Buffer>(destinationBuffer),
			copyRegions
		));
	}

	void GL::CommandEncoder::copyBufferToImage(ref<ProjectA::Buffer> sourceBuffer, ref<ProjectA::Image> destinationImage, const sequence<BufferImageCopyRegion>& copyRegions) {
		commands.emplace_back(createRef<GL::CopyBufferToImageCommand>(
			std::static_pointer_cast<GL::Buffer>(sourceBuffer),
			std::static_pointer_cast<GL::Image>(destinationImage),
			copyRegions
		));
	}

	void GL::CommandEncoder::copyImageToBuffer(ref<ProjectA::Image> sourceImage, ref<ProjectA::Buffer> destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) {
		commands.emplace_back(createRef<GL::CopyImageToBufferCommand>(
			std::static_pointer_cast<GL::Image>(sourceImage),
			std::static_pointer_cast<GL::Buffer>(destinationBuffer),
			copyRegions
		));
	}

	void GL::CommandEncoder::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		ADAPTER_ASSERT(groupCountX <= limits.maxComputeWorkgroupCount[0], "Group count in the X axis must not exceed [].", limits.maxComputeWorkgroupCount[0]);
		ADAPTER_ASSERT(groupCountY <= limits.maxComputeWorkgroupCount[1], "Group count in the Y axis must not exceed [].", limits.maxComputeWorkgroupCount[1]);
		ADAPTER_ASSERT(groupCountZ <= limits.maxComputeWorkgroupCount[2], "Group count in the Z axis must not exceed [].", limits.maxComputeWorkgroupCount[2]);

		commands.emplace_back(createRef<GL::DispatchCommand>(groupCountX, groupCountY, groupCountZ));
	}

	void GL::CommandEncoder::dispatchIndirect(ref<ProjectA::Buffer> buffer, uint64 bufferOffset) {
		commands.emplace_back(createRef<GL::DispatchIndirectCommand>(std::static_pointer_cast<GL::Buffer>(buffer)->buffer, bufferOffset));
	}

	void GL::CommandEncoder::nextSubpass() {
		subpassIndex += 1;

		ADAPTER_ASSERT(subpassIndex < renderPass->getSubpassCount(), "All subpasses have been executed.");

		commands.emplace_back(createRef<GL::NextSubpassCommand>(framebuffer, renderPass->getSubpass(subpassIndex)));
	}

	void GL::CommandEncoder::setImageMemoryBarrier(ref<ProjectA::Image> image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) {
		const GLbitfield barrier = getBarrier(image->getLayout());

		if (barrier == 0) {
			return;
		}

		commands.emplace_back(createRef<GL::SetImageMemoryBarrierCommand>(barrier));

		image->setLayout(newLayout);
	}

	void GL::CommandEncoder::setLineWidth(float32 lineWidth) {
		commands.emplace_back(createRef<GL::SetLineWidthCommand>(lineWidth));
	}

	void GL::CommandEncoder::setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) {
		commands.emplace_back(createRef<GL::SetScissorCommand>(xOffset, yOffset, extent));
	}

	void GL::CommandEncoder::setViewport(vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) {
		commands.emplace_back(createRef<GL::SetViewportCommand>(
			static_cast<GLint>(origin[0]),

			// TODO: Allow the origin's Y to be flipped, like in Vulkan?
			0,

			static_cast<GLsizei>(abs(size[0])),
			static_cast<GLsizei>(abs(size[1]))
		));
	}

	void GL::CommandEncoder::draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		commands.emplace_back(createRef<GL::DrawCommand>(
			renderPipeline->primitiveTopology,
			vertexOffset,
			vertexCount,
			instanceCount,
			instanceOffset
		));
	}

	void GL::CommandEncoder::drawIndirect(ref<ProjectA::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		ADAPTER_ASSERT(buffer->usage & BufferUsage::INDIRECT, "This buffer cannot be used as an indirect buffer.");
		ADAPTER_ASSERT(drawCount > 0, "A DrawIndirect command must result in at least 1 draw call.");

		commands.emplace_back(createRef<GL::DrawIndirectCommand>(
			renderPipeline->primitiveTopology,
			std::static_pointer_cast<GL::Buffer>(buffer)->buffer,
			(const GLubyte*) bufferOffset,
			drawCount,
			stride
		));
	}

	void GL::CommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		const uint64 indices = this->indexOffset + indexOffset * indexTypeSize;

		commands.emplace_back(createRef<GL::DrawIndexedCommand>(
			renderPipeline->primitiveTopology,
			indexCount,
			indexType,
			(const GLvoid*) indices,
			instanceCount,
			indexBias,
			instanceOffset
		));
	}

	void GL::CommandEncoder::drawIndexedIndirect(ref<ProjectA::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		ADAPTER_ASSERT(buffer->usage & BufferUsage::INDIRECT, "This buffer cannot be used as an indirect buffer.");
		ADAPTER_ASSERT(drawCount > 0, "A DrawIndirect command must result in at least 1 draw call.");

		commands.emplace_back(createRef<GL::DrawIndexedIndirectCommand>(
			renderPipeline->primitiveTopology,
			indexType,
			std::static_pointer_cast<GL::Buffer>(buffer)->buffer,
			(const GLubyte*) bufferOffset,
			drawCount,
			stride
		));
	}

	void GL::CommandEncoder::endRenderPass() {
		commands.emplace_back(createRef<GL::EndRenderPassCommand>(framebuffer));
	}

	ref<CommandBuffer> GL::CommandEncoder::end() const {
		return createRef<GL::CommandBuffer>(commands);
	}

	void GL::CommandEncoder::reset() {
		commands.clear();
		framebuffer = nullptr;
		renderPass = nullptr;
		subpassIndex = 0;
		computePipeline = nullptr;
		renderPipeline = nullptr;
		indexOffset = 0;
		indexType = 0;
		indexTypeSize = 0;
	}

	void GL::CommandEncoder::bindIndexBuffer(ref<ProjectA::Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		indexOffset = static_cast<uint32>(offset);
		indexType = getIndexType(indexTypeSize);
		this->indexTypeSize = indexTypeSize;

		commands.emplace_back(createRef<GL::BindIndexBufferCommand>(
			renderPipeline->vertexArray,
			std::static_pointer_cast<GL::Buffer>(indexBuffer)->buffer
		));
	}
}