// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	void Physics2DRenderer::encodeOneTimeCommands() const {
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

		vec2<float32>* vertexMappedRange = vertexStagingView.map();
		uint16* indexMappedRange = indexStagingView.map();

		std::copy_n(CIRCLE_VERTICES, 4, vertexMappedRange);
		std::copy_n(CIRCLE_INDICES, 6, indexMappedRange);

		vertexMappedRange += 4;
		indexMappedRange += 6;

		for (const PhysicsRendererBuildHull2D& hull : hulls) {
			const vec2<float32>* hullVertices = pointer_cast<const vec2<float32>*>(pointer_cast<const uint8*>(hull.hullHeader) + hull.hullHeader->vertexOffset);

			std::copy_n(hullVertices, hull.hullHeader->vertexCount, vertexMappedRange);

			indexMappedRange[0] = 0;

			const uint32 halfVertexCount = hull.hullHeader->vertexCount / 2;

			uint16 index = 1;

			while (index < hull.hullHeader->vertexCount) {
				const uint16 x = (index + (index & 1)) / 2;

				if (index & 1) {
					indexMappedRange[index] = x;
				}
				else {
					indexMappedRange[hull.hullHeader->vertexCount - index] = halfVertexCount + x;
				}

				index += 1;
			}

			index = 0;

			while (index < hull.hullHeader->vertexCount) {
				indexMappedRange[hull.hullHeader->vertexCount + index] = index;

				index += 1;
			}

			indexMappedRange[hull.hullHeader->vertexCount + index] = 0;

			vertexMappedRange += hull.hullHeader->vertexCount;
			indexMappedRange += hull.hullHeader->vertexCount * 2 + 1;
		}

		stagingBuffer->unmap();

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