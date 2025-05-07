#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLBeginRenderPassCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLBindComputePipelineCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLBindIndexBufferCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLBindRenderPipelineCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLBindVertexBufferCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLCopyBufferToBufferCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLCopyBufferToImageCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLDispatchGroupsCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLDrawCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLDrawIndexedCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLSetImageMemoryBarrierCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLSetViewportCommand.hpp"
#include "Backend/OpenGL/IndexType.hpp"
#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"
#include "Backend/OpenGL/OpenGLCommandEncoder.hpp"
#include "Backend/OpenGL/OpenGLImage.hpp"

namespace ProjectA {
	OpenGLCommandEncoder::OpenGLCommandEncoder(const OpenGLDevice& device, const CommandBufferDescriptor& descriptor) :
		limits(device.getPhysicalDevice()->getLimits())
	{}

	void OpenGLCommandEncoder::begin() const {}

	void OpenGLCommandEncoder::beginRenderPass(const BeginRenderPassDescriptor& descriptor) {
		commandBuffer.emplace_back(createRef<OpenGLBeginRenderPassCommand>(descriptor));
	}

	void OpenGLCommandEncoder::bindComputePipeline(const ref<ComputePipeline>& computePipeline) {
		this->computePipeline = std::static_pointer_cast<OpenGLComputePipeline>(computePipeline);

		commandBuffer.emplace_back(createRef<OpenGLBindComputePipelineCommand>(this->computePipeline));
	}

	void OpenGLCommandEncoder::bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) {
		this->renderPipeline = std::static_pointer_cast<OpenGLRenderPipeline>(renderPipeline);

		commandBuffer.emplace_back(createRef<OpenGLBindRenderPipelineCommand>(this->renderPipeline));
	}

	void OpenGLCommandEncoder::bindAllDescriptorSets() const {}

	void OpenGLCommandEncoder::bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset, uint8 indexTypeSize) {
		indexOffset = static_cast<uint32>(offset);
		indexType = getGlIndexType(indexTypeSize);
		this->indexTypeSize = indexTypeSize;

		commandBuffer.emplace_back(createRef<OpenGLBindIndexBufferCommand>(
			std::static_pointer_cast<OpenGLBuffer>(indexBuffer)->object
		));
	}

	void OpenGLCommandEncoder::bindVertexBuffer(uint32 bindingIndex, const ref<Buffer>& vertexBuffer, uint64 offset, uint32 stride) {
		commandBuffer.emplace_back(createRef<OpenGLBindVertexBufferCommand>(
			bindingIndex,
			std::static_pointer_cast<OpenGLBuffer>(vertexBuffer)->object,
			offset,
			stride
		));
	}

	void OpenGLCommandEncoder::copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, uint64 sourceOffset, uint64 destinationOffset, uint64 size) {
		ENGINE_ASSERT(size > 0, "OpenGL: The size for a buffer copy operation must be non-zero.");

		commandBuffer.emplace_back(createRef<OpenGLCopyBufferToBufferCommand>(
			std::static_pointer_cast<OpenGLBuffer>(sourceBuffer)->object,
			std::static_pointer_cast<OpenGLBuffer>(destinationBuffer)->object,
			sourceOffset,
			destinationOffset,
			size
		));
	}

	void OpenGLCommandEncoder::copyBufferToImage(const ref<Buffer>& sourceBuffer, const ref<Image>& destinationImage, uint32 width, uint32 height, uint32 depth) {
		const ref<OpenGLImage> glImage = std::static_pointer_cast<OpenGLImage>(destinationImage);

		commandBuffer.emplace_back(createRef<OpenGLCopyBufferToImageCommand>(
			std::static_pointer_cast<OpenGLBuffer>(sourceBuffer)->object,
			glImage->object,
			width,
			height,
			depth,
			glImage->format
		));
	}

	void OpenGLCommandEncoder::dispatchGroups(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		ENGINE_ASSERT(groupCountX <= limits.maxComputeWorkgroupCount[0], "Group count in the X axis must not exceed [].", limits.maxComputeWorkgroupCount[0]);
		ENGINE_ASSERT(groupCountY <= limits.maxComputeWorkgroupCount[1], "Group count in the Y axis must not exceed [].", limits.maxComputeWorkgroupCount[1]);
		ENGINE_ASSERT(groupCountZ <= limits.maxComputeWorkgroupCount[2], "Group count in the Z axis must not exceed [].", limits.maxComputeWorkgroupCount[2]);

		commandBuffer.emplace_back(createRef<OpenGLDispatchGroupsCommand>(groupCountX, groupCountY, groupCountZ));
	}

	void OpenGLCommandEncoder::setImageMemoryBarrier(const ref<Image>& image, ImageLayout oldLayout, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) {
		GLbitfield barrier = 0;

		switch (newLayout) {
			case ImageLayout::SHADER_READ_ONLY_OPTIMAL:
				barrier = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;

				break;
		}

		if (barrier == 0) {
			return;
		}

		commandBuffer.emplace_back(createRef<OpenGLSetImageMemoryBarrierCommand>(barrier));
	}

	void OpenGLCommandEncoder::setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) {
		DEBUG_ASSERT(renderPipeline->dynamicState & DynamicState::VIEWPORT);

		commandBuffer.emplace_back(createRef<OpenGLSetViewportCommand>(
			static_cast<GLint>(origin[0]),

			// TODO: Use Y origin.
			0,

			static_cast<GLsizei>(lyah::abs(size[0])),
			static_cast<GLsizei>(lyah::abs(size[1]))
		));
	}

	void OpenGLCommandEncoder::setScissor(sint32 xOffset, sint32 yOffset, uint32 width, uint32 height) {
		DEBUG_ASSERT(renderPipeline->dynamicState & DynamicState::SCISSOR);

		// TODO: Implement setScissor command.
	}

	void OpenGLCommandEncoder::draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset) {
		commandBuffer.emplace_back(createRef<OpenGLDrawCommand>(
			renderPipeline->primitiveTopology,
			vertexOffset,
			vertexCount,
			instanceCount
		));
	}

	void OpenGLCommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset) {
		const uint32 indices = this->indexOffset + indexOffset * indexTypeSize;

		commandBuffer.emplace_back(createRef<OpenGLDrawIndexedCommand>(
			renderPipeline->primitiveTopology,
			indexCount,
			indexType,
			(const GLvoid*) (const std::uintptr_t) indices,
			instanceCount
		));
	}

	void OpenGLCommandEncoder::endRenderPass() const {}

	ref<CommandBuffer> OpenGLCommandEncoder::end() const {
		return createRef<OpenGLCommandBuffer>(commandBuffer);
	}

	void OpenGLCommandEncoder::reset() {
		commandBuffer.clear();
		computePipeline = nullptr;
		renderPipeline = nullptr;
		indexOffset = 0;
		indexType = 0;
		indexTypeSize = 0;
	}
}