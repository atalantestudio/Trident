// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	void Physics2DRenderer::copyGeometry() const {
		static constexpr vec2<float32> CIRCLE_VERTICES[] = {
			{-0.5f,  0.5f},
			{ 0.5f,  0.5f},
			{-0.5f, -0.5f},
			{ 0.5f, -0.5f},
		};

		static constexpr uint16 CIRCLE_INDICES[] = {
			0, 1, 2,
			1, 3, 2,
		};

		const ref<Buffer> stagingBuffer = createDynamicBuffer({
			.size = vertexBuffer->getSize() + indexBuffer->getSize(),
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Staging",
			#endif
		});
		const BufferView<vec2<float32>> vertexStagingView = stagingBuffer->createView<vec2<float32>>(0, vertexBuffer->getSize());
		const BufferView<uint16> indexStagingView = stagingBuffer->createView<uint16>(vertexStagingView.getOffset() + vertexStagingView.getSize(), indexBuffer->getSize());

		std::copy_n(CIRCLE_VERTICES, 4, vertexStagingView.map());
		std::copy_n(CIRCLE_INDICES, 6, indexStagingView.map());

		const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "One-time commands",
			#endif
		});

		commandEncoder->begin();

		const BufferCopyRegion bufferCopyRegions[] = {
			{
				.sourceOffset = vertexStagingView.getOffset(),
				.destinationOffset = 0,
				.size = vertexStagingView.getSize(),
			},
			{
				.sourceOffset = indexStagingView.getOffset(),
				.destinationOffset = 0,
				.size = indexStagingView.getSize(),
			},
		};

		commandEncoder->copyBufferToBuffer(stagingBuffer, vertexBuffer, bufferCopyRegions[0]);
		commandEncoder->copyBufferToBuffer(stagingBuffer, indexBuffer, bufferCopyRegions[1]);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();
	}
}