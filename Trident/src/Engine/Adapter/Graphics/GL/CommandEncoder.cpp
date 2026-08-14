// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Barrier.hpp"
#include "Engine/Adapter/Graphics/GL/Command/BeginRenderPassCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/BindComputePipelineCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/BindDescriptorSetsCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/BindIndexBufferCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/BindGraphicsPipelineCommand.hpp"
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
#include "Engine/Adapter/Graphics/GL/Command/InsertMemoryBarriersCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/NextSubpassCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/SetLineWidthCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/SetScissorCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Command/SetViewportCommand.hpp"
#include "Engine/Adapter/Graphics/GL/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/GL/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageLayout.hpp"
#include "Engine/Adapter/Graphics/GL/IndexType.hpp"

namespace Trident {
	GL::CommandEncoder::CommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool) :
		Trident::CommandEncoder(device, commandPool)
		#if defined(GL_QUEUE_COMMANDS)
			, commands(GL_COMMAND_QUEUE_SIZE)
		#endif
	{}

	template<typename CommandType>
	void GL::CommandEncoder::addCommand(ref<CommandType> command) {
		#if defined(GL_QUEUE_COMMANDS)
			commands[commandCount] = command;
			commandCount += 1;
		#else
			command->execute();
		#endif
	}

	void GL::begin(const Trident::CommandEncoder* commandEncoder) {}

	void GL::beginRenderPass(Trident::CommandEncoder* commandEncoder, const BeginRenderPassDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->framebuffer = static_pointer_cast<GL::Framebuffer>(descriptor.framebuffer);
		_commandEncoder->renderPass = static_pointer_cast<GL::RenderPass>(descriptor.framebuffer->getRenderPass()); 
		_commandEncoder->addCommand(createRef<GL::BeginRenderPassCommand>(descriptor));
	}

	void GL::bindComputePipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::ComputePipeline> computePipeline) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->computePipeline = static_pointer_cast<GL::ComputePipeline>(computePipeline);
		_commandEncoder->addCommand(createRef<GL::BindComputePipelineCommand>(_commandEncoder->computePipeline));
	}

	void GL::bindDescriptorSets(Trident::CommandEncoder* commandEncoder, ref<Trident::PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<Trident::DescriptorSet>> descriptorSets) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);

		// TODO(graphics/gl/descriptor-set/binding-offset)
		_commandEncoder->addCommand(createRef<GL::BindDescriptorSetsCommand>(descriptorSets));
	}

	void GL::bindGraphicsPipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::GraphicsPipeline> graphicsPipeline) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->graphicsPipeline = static_pointer_cast<GL::GraphicsPipeline>(graphicsPipeline);

		ADAPTER_ASSERT(_commandEncoder->graphicsPipeline->subpassIndex == _commandEncoder->subpassIndex, "The pipeline must be associated to the current subpass.");

		_commandEncoder->addCommand(createRef<GL::BindGraphicsPipelineCommand>(_commandEncoder->graphicsPipeline));
	}

	void GL::bindIndexBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->indexOffset = static_cast<uint32>(offset);
		_commandEncoder->indexType = getIndexType(indexTypeSize);
		_commandEncoder->indexTypeSize = indexTypeSize;
		_commandEncoder->addCommand(createRef<GL::BindIndexBufferCommand>(
			_commandEncoder->graphicsPipeline->vertexArray,
			static_pointer_cast<GL::Buffer>(indexBuffer)->buffer
		));
	}

	void GL::bindVertexBuffer(Trident::CommandEncoder* commandEncoder, uint32 bindingIndex, ref<Trident::Buffer> vertexBuffer, uint64 offset) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::BindVertexBufferCommand>(
			_commandEncoder->graphicsPipeline->vertexArray,
			bindingIndex,
			static_pointer_cast<GL::Buffer>(vertexBuffer)->buffer,
			offset,
			_commandEncoder->graphicsPipeline->vertexInputBindingStrides[bindingIndex]
		));
	}

	void GL::copyBufferToBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> sourceBuffer, ref<Trident::Buffer> destinationBuffer, view<BufferCopyRegion> copyRegions) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::CopyBufferToBufferCommand>(
			static_pointer_cast<GL::Buffer>(sourceBuffer),
			static_pointer_cast<GL::Buffer>(destinationBuffer),
			copyRegions
		));
	}

	void GL::copyBufferToImage(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> sourceBuffer, ref<Trident::Image> destinationImage, view<BufferImageCopyRegion> copyRegions) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::CopyBufferToImageCommand>(
			static_pointer_cast<GL::Buffer>(sourceBuffer),
			static_pointer_cast<GL::Image>(destinationImage),
			copyRegions
		));
	}

	void GL::copyImageToBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::Image> sourceImage, ref<Trident::Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::CopyImageToBufferCommand>(
			static_pointer_cast<GL::Image>(sourceImage),
			static_pointer_cast<GL::Buffer>(destinationBuffer),
			copyRegions
		));
	}

	ref<Trident::CommandEncoder> GL::createCommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const CommandBufferDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::CommandEncoder> commandEncoder = allocateRef<GL::CommandEncoder>(device, commandPool);
		commandEncoder->limits = device->getPhysicalDevice()->getLimits();

		return commandEncoder;
	}

	void GL::destroyCommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const Trident::CommandEncoder* commandEncoder) {}

	void GL::dispatch(Trident::CommandEncoder* commandEncoder, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::DispatchCommand>(groupCountX, groupCountY, groupCountZ));
	}

	void GL::dispatchIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> buffer, uint64 bufferOffset) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::DispatchIndirectCommand>(static_pointer_cast<GL::Buffer>(buffer)->buffer, bufferOffset));
	}

	void GL::draw(Trident::CommandEncoder* commandEncoder, uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::DrawCommand>(
			_commandEncoder->graphicsPipeline->primitiveTopology,
			vertexOffset,
			vertexCount,
			instanceCount,
			instanceOffset
		));
	}

	void GL::drawIndexed(Trident::CommandEncoder* commandEncoder, uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		const uint64 indices = _commandEncoder->indexOffset + indexOffset * _commandEncoder->indexTypeSize;

		_commandEncoder->addCommand(createRef<GL::DrawIndexedCommand>(
			_commandEncoder->graphicsPipeline->primitiveTopology,
			indexCount,
			_commandEncoder->indexType,
			(const GLvoid*) indices,
			instanceCount,
			indexBias,
			instanceOffset
		));
	}

	void GL::drawIndexedIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::DrawIndexedIndirectCommand>(
			_commandEncoder->graphicsPipeline->primitiveTopology,
			_commandEncoder->indexType,
			static_pointer_cast<GL::Buffer>(buffer)->buffer,
			(const GLubyte*) bufferOffset,
			drawCount,
			stride
		));
	}

	void GL::drawIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::DrawIndirectCommand>(
			_commandEncoder->graphicsPipeline->primitiveTopology,
			static_pointer_cast<GL::Buffer>(buffer)->buffer,
			(const GLubyte*) bufferOffset,
			drawCount,
			stride
		));
	}

	ref<Trident::CommandBuffer> GL::end(const Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::CommandEncoder* _commandEncoder = static_cast<const GL::CommandEncoder*>(commandEncoder);

		view<ref<GL::Command>> commands;

		#if defined(GL_QUEUE_COMMANDS)
			commands = view<ref<GL::Command>>(&_commandEncoder->commands[0], _commandEncoder->commandCount);
		#endif

		return createRef<GL::CommandBuffer>(commands);
	}

	void GL::endRenderPass(Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::EndRenderPassCommand>(_commandEncoder->framebuffer));
	}

	void GL::insertMemoryBarriers(Trident::CommandEncoder* commandEncoder, view<BufferMemoryBarrier> bufferMemoryBarriers, view<ImageMemoryBarrier> imageMemoryBarriers) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		GLbitfield barriers = 0;
		uint32 bufferMemoryBarrierIndex = 0;

		while (bufferMemoryBarrierIndex < bufferMemoryBarriers.count()) {
			const BufferMemoryBarrier& bufferMemoryBarrier = bufferMemoryBarriers[bufferMemoryBarrierIndex];

			barriers |= getBarrier(bufferMemoryBarrier.newAccess);

			bufferMemoryBarrier.buffer->access = bufferMemoryBarrier.newAccess;

			bufferMemoryBarrierIndex += 1;
		}

		uint32 imageMemoryBarrierIndex = 0;

		while (imageMemoryBarrierIndex < imageMemoryBarriers.count()) {
			const ImageMemoryBarrier& imageMemoryBarrier = imageMemoryBarriers[imageMemoryBarrierIndex];

			barriers |= getBarrier(imageMemoryBarrier.image->getLayout());

			imageMemoryBarrier.image->layout = imageMemoryBarrier.newLayout;
			imageMemoryBarrier.image->access = imageMemoryBarrier.newAccess;

			imageMemoryBarrierIndex += 1;
		}

		if (barriers == 0) {
			return;
		}

		_commandEncoder->addCommand(createRef<GL::InsertMemoryBarriersCommand>(barriers));
	}

	void GL::nextSubpass(Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->subpassIndex += 1;

		ADAPTER_ASSERT(_commandEncoder->subpassIndex < _commandEncoder->renderPass->getSubpassCount(), "All subpasses have been executed.");

		_commandEncoder->addCommand(createRef<GL::NextSubpassCommand>(_commandEncoder->framebuffer, _commandEncoder->subpassIndex));
	}

	void GL::reset(Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->framebuffer = nullptr;
		_commandEncoder->renderPass = nullptr;
		_commandEncoder->subpassIndex = 0;

		#if defined(GL_QUEUE_COMMANDS)
			// _commandEncoder->commands = sequence<ref<GL::Command>>(GL_COMMAND_QUEUE_SIZE);
			_commandEncoder->commandCount = 0;
		#endif

		_commandEncoder->computePipeline = nullptr;
		_commandEncoder->graphicsPipeline = nullptr;
		_commandEncoder->indexOffset = 0;
		_commandEncoder->indexType = 0;
		_commandEncoder->indexTypeSize = 0;
	}

	void GL::setLineWidth(Trident::CommandEncoder* commandEncoder, float32 lineWidth) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::SetLineWidthCommand>(lineWidth));
	}

	void GL::setScissor(Trident::CommandEncoder* commandEncoder, sint32 xOffset, sint32 yOffset, Extent2D extent) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::SetScissorCommand>(xOffset, yOffset, extent));
	}

	void GL::setViewport(Trident::CommandEncoder* commandEncoder, vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		GL::CommandEncoder* _commandEncoder = static_cast<GL::CommandEncoder*>(commandEncoder);
		_commandEncoder->addCommand(createRef<GL::SetViewportCommand>(origin, size, minDepth, maxDepth));
	}
}